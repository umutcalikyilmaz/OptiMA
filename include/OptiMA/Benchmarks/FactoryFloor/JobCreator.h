#pragma once
#include <fstream>
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"

class JobCreator
{
public:

    JobCreator(std::string filePath) : useExisting_(true), filePath_(filePath), txnRandom_(0,1), operationRandom_(0,1),
    probabilityRandom_(0,1) { }

    JobCreator(int jobNum, int minTransactionNum, int maxTransactionNum, int minOperationNum, int maxOperationNum,
    double manualOperationsCoef, double drillingCoef, double weldingCoef, std::vector<double>& manualOperationCoefs,
    std::vector<double>& drillingOperationCoefs, std::vector<double>& weldingOperationsCoefs) : useExisting_(false),
    saveJobs_(false), jobNum_(jobNum), txnRandom_(minTransactionNum, maxTransactionNum + 1), 
    operationRandom_(minOperationNum, maxOperationNum + 1), probabilityRandom_(0, 1)
    {
        initialize(manualOperationsCoef, drillingCoef, weldingCoef, manualOperationCoefs, drillingOperationCoefs,
        weldingOperationsCoefs);
    }

    JobCreator(int jobNum, int minTransactionNum, int maxTransactionNum, int minOperationNum, int maxOperationNum,
    double manualOperationsCoef, double drillingCoef, double weldingCoef, std::vector<double>& manualOperationCoefs,
    std::vector<double>& drillingOperationCoefs, std::vector<double>& weldingOperationsCoefs, std::string filePath)
    : useExisting_(false), saveJobs_(true), filePath_(filePath), jobNum_(jobNum), txnRandom_(minTransactionNum,
    maxTransactionNum + 1), operationRandom_(minOperationNum, maxOperationNum + 1), probabilityRandom_(0, 1)
    {
        initialize(manualOperationsCoef, drillingCoef, weldingCoef, manualOperationCoefs, drillingOperationCoefs,
        weldingOperationsCoefs);
    }
  
    std::shared_ptr<std::vector<std::unique_ptr<Job>>> createJobs()
    {
        std::shared_ptr<std::vector<std::unique_ptr<Job>>> res = std::make_shared<std::vector<std::unique_ptr<Job>>>();
        int c = 0;
    
        if(useExisting_)
        {
            std::fstream file;
            file.open(filePath_, std::fstream::in);
            std::string line;
    
            while(getline(file, line)) 
            {
                std::unique_ptr<Job> job = std::make_unique<Job>();
                job->id = c++;
                job->operationTypes.push(std::vector<std::pair<OperationType, int>>());

                int size = line.size();
                int pos = 0;
                int st = 0;
                bool first = true;
                OperationType firstElement;                

                while(pos < size)
                {
                    if(line.at(pos) == '|')
                    {
                        firstElement = (OperationType)stoi(line.substr(st, pos - st));
                        st = pos + 1;
                    }
                    else if(line.at(pos) == ',')
                    {
                        job->operationTypes.back().push_back(std::make_pair(firstElement, stoi(line.substr(st, pos - st))));
                        st = pos + 1;
                    }
                    else if(line.at(pos) == ';')
                    {
                        job->operationTypes.back().push_back(std::make_pair(firstElement, stoi(line.substr(st, pos - st))));

                        if(pos < size - 1)
                        {                            
                            job->operationTypes.push(std::vector<std::pair<OperationType, int>>());
                        }
                        
                        st = pos + 1;
                    }

                    pos++;
                }

                res->push_back(move(job));
            }

            file.close();
        }
        else
        {
            std::fstream file;
    
            if(saveJobs_)
            {
                file.open(filePath_, std::fstream::out | std::fstream::trunc);
            }
    
            for(int i = 0; i < jobNum_; i++)
            {            
                std::unique_ptr<Job> job = std::make_unique<Job>();
                job->id = c++;
                int txnNum = (int)txnRandom_.generate();

                for(int j = 0; j < txnNum; j++)
                {
                    job->operationTypes.push(std::vector<std::pair<OperationType,int>>());
                    int operationNum = (int)operationRandom_.generate();

                    for(int k = 0; k < operationNum; k++)
                    {
                        double p = probabilityRandom_.generate();

                        if(p < manualProb_)
                        {
                            double p = probabilityRandom_.generate();

                            for(int l = 0; l < 5; l++)
                            {
                                if(p < manualOperationProbs_[l])
                                {
                                    job->operationTypes.back().push_back(std::make_pair(MANUAL, l));
                                    break;
                                }
                            }
                        }
                        else if(p < drillingProb_)
                        {
                            double p = probabilityRandom_.generate();

                            if(p < drillingOperation1Prob)
                            {
                                job->operationTypes.back().push_back(std::make_pair(DRILLING, 0));
                            }
                            else
                            {
                                job->operationTypes.back().push_back(std::make_pair(DRILLING, 1));
                            }
                        }
                        else
                        {
                            double p = probabilityRandom_.generate();

                            if(p < weldingOperation1Prob)
                            {
                                job->operationTypes.back().push_back(std::make_pair(WELDING, 0));
                            }
                            else
                            {
                                job->operationTypes.back().push_back(std::make_pair(WELDING, 1));
                            }
                        }

                        if(saveJobs_)
                        {
                            file << std::to_string(job->operationTypes.back().back().first) << "|" <<
                                std::to_string(job->operationTypes.back().back().second);

                            if(k < operationNum - 1)
                            {
                                file << ",";
                            }
                        }
                    }

                    if(saveJobs_)
                    {
                        file << ";";
                    }
                }           
        
                if(saveJobs_)
                {
                    file << "\n";
                }

                res->push_back(move(job));
            }
    
            if(saveJobs_)
            {
                file.close();
            }
        } 
        
        return res;
    }

private:

    UniformRandom txnRandom_;
    UniformRandom operationRandom_;
    UniformRandom probabilityRandom_;
    std::vector<double> manualOperationProbs_;
    std::string filePath_; 
    double drillingOperation1Prob;
    double weldingOperation1Prob;       
    double manualProb_;
    double drillingProb_;  
    int jobNum_;
    bool useExisting_;
    bool saveJobs_;

    void initialize(double manualOperationsCoef, double drillingCoef, double weldingCoef,
    std::vector<double>& manualOperationCoefs, std::vector<double>& drillingOperationCoefs,
    std::vector<double>& weldingOperationsCoefs)
    {
        double tot = manualOperationsCoef + drillingCoef + weldingCoef;
        manualProb_ = manualOperationsCoef / tot;
        drillingProb_ = drillingCoef / tot + manualProb_;

        tot = 0;

        for(int i = 0; i < 5; i++)
        {
            tot += manualOperationCoefs[i];
        }

        manualOperationProbs_.push_back(manualOperationCoefs[0] / tot);

        for(int i = 1; i < 4; i++)
        {
            manualOperationProbs_.push_back(manualOperationProbs_[i - 1] +  manualOperationCoefs[i] / tot);
        }

        manualOperationProbs_.push_back(1);

        drillingOperation1Prob = drillingOperationCoefs[0] / (drillingOperationCoefs[0] + drillingOperationCoefs[1]);
        weldingOperation1Prob = weldingOperationsCoefs[0] / (weldingOperationsCoefs[0] + weldingOperationsCoefs[1]);
    }
};