#pragma once
#include <fstream>
#include <Benchmarks/FactoryFloor/Job.h>
#include <Benchmarks/Shared/RandomNumber.h>

class JobCreator
{
    UniformRandom rnd;
    UniformRandom rnd2;
    vector<double> manualOperationProbs;
    string filePath;    
    double manualOperationsProb;
    double drillingProb;
    double weldingProb;    
    int jobNum;
    bool useExisting;
    bool saveJobs;

    void Initialize(double manualOperationsCoef, double drillingCoef, double weldingCoef,
    vector<double>& manualOperationCoefs)
    {
        double tot = manualOperationsCoef + drillingCoef + weldingCoef;
        manualOperationsProb = manualOperationsCoef / tot;
        drillingProb = drillingCoef / tot;
        weldingProb = weldingCoef / tot;
    
        tot = 0;
    
        for(int i = 0; i < 5; i++)
        {
            tot += manualOperationCoefs[i];
            manualOperationProbs.push_back(0);
        }

        manualOperationProbs[0] = manualOperationCoefs[0] / tot;
    
        for(int i = 1; i < 4; i++)
        {
            manualOperationProbs[i] = manualOperationProbs[i - 1] +  manualOperationCoefs[i] / tot;
        }
    
        manualOperationProbs[4] = 1;
    }

public:

    JobCreator(string filePath) : useExisting(true), filePath(filePath), rnd(0,1), rnd2(0,1) { }

    JobCreator(int jobNum, int minOperationNum, int maxOperationNum, double manualOperationsCoef, double drillingCoef,
    double weldingCoef, vector<double>& manualOperationCoefs) : useExisting(false), saveJobs(false),
    jobNum(jobNum), rnd(minOperationNum, maxOperationNum + 1), rnd2(0, 1)
    {
        Initialize(manualOperationsCoef, drillingCoef, weldingCoef, manualOperationCoefs);
    }


    JobCreator(int jobNum, int minOperationNum, int maxOperationNum, double manualOperationsCoef, double drillingCoef,
    double weldingCoef, vector<double>& manualOperationCoefs, string filePath) : useExisting(false), saveJobs(true), filePath(filePath), jobNum(jobNum), rnd(minOperationNum,
    maxOperationNum + 1), rnd2(0, 1)
    {
        Initialize(manualOperationsCoef, drillingCoef, weldingCoef, manualOperationCoefs);
    }
  
    shared_ptr<vector<unique_ptr<Job>>> CreateJobs()
    {
        shared_ptr<vector<unique_ptr<Job>>> res = make_shared<vector<unique_ptr<Job>>>();
        int c = 0;
    
        if(useExisting)
        {
            fstream file;
            file.open(filePath, fstream::in);
            string line;
    
            while(getline(file, line))
            {
                unique_ptr<Job> job = make_unique<Job>();
                job->id = c++;
    
                int size = line.size();
                int pos = 0;
                int st = 0;
                bool first = true;
                OperationType firstElement;
    
                while(pos < size)
                {    
                    if(line.at(pos) == ',')
                    {
                        if(first)
                        {
                            firstElement = (OperationType)stoi(line.substr(st, pos - st));
                            first = false;                     
                        }
                        else
                        {
                            job->operationTypes.push(make_pair(firstElement, stoi(line.substr(st, pos - st))));
                            first = true;
                        }
    
                        st = pos + 1;
                    }
                    else if(pos == size - 1)
                    {
                        job->operationTypes.push(make_pair(firstElement, stoi(line.substr(st, size - st))));
                    }
    
                    pos++;
                }
    
                res->push_back(move(job));
            }

            file.close();
        }
        else
        {
            fstream file;
    
            if(saveJobs)
            {
                file.open(filePath, fstream::out | fstream::trunc);
            }
    
            for(int i = 0; i < jobNum; i++)
            {            
                unique_ptr<Job> job = make_unique<Job>();
                job->id = c++;
                int operationNum = (int)rnd.Generate();            
        
                for(int j = 0; j < operationNum; j++)
                {
                    double p = rnd2.Generate();
        
                    if(p < manualOperationsProb)
                    {
                        p = rnd2.Generate();
        
                        for(int k = 0; k < 5; k++)
                        {
                            if(p <= manualOperationProbs[k])
                            {
                                job->operationTypes.push(make_pair(MANUAL, k + 1));
                                break;
                            }
                        }
        
                    }
                    else if(p < manualOperationsProb + drillingProb)
                    {
                        job->operationTypes.push(make_pair(DRILLING, 0));
                    }
                    else
                    {
                        job->operationTypes.push(make_pair(WELDING, 0));
                    }
    
                    if(saveJobs)
                    {                    
                        file << to_string(job->operationTypes.back().first) << "," << to_string(job->operationTypes.back().second);
    
                        if(j != operationNum - 1)
                        {
                            file << ",";
                        }
                    }
    
                }
    
                if(saveJobs)
                {
                    file << "\n";
                }
    
                res->push_back(move(job));
            }
    
            if(saveJobs)
            {
                file.close();
            }
        }  
    
        return res;
    }
};