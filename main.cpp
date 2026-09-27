#include <iostream>
#include "OptiMA/FactoryFloor.h"

int main(int, char**)
{
    std::vector<double> timeScales {0.1};
    std::vector<std::pair<TxnSP::TemperatureEvolution, double>> decrementSettings
    {
        //make_pair(TxnSP::Linear, 1),
        //make_pair(TxnSP::Linear, 0.1),
        //make_pair(TxnSP::Linear, 0.05),
        //make_pair(TxnSP::TemperatureEvolution::Linear, 0.001),
        //make_pair(TxnSP::TemperatureEvolution::Linear, 0.0005),
        std::make_pair(TxnSP::TemperatureEvolution::Linear, 0.005),
        //make_pair(TxnSP::TemperatureEvolution::Linear, 1.2),
        //make_pair(TxnSP::Exponential, 0.9),
        //make_pair(TxnSP::Exponential, 0.99)
    };
    std::vector<double> temperatures {1.2};
    std::vector<int> batchSizes {50, 100, 200};
    std::vector<std::chrono::milliseconds> timeouts {std::chrono::milliseconds(500), std::chrono::milliseconds(1000)};
    std::vector<int> threadNumbers {8, 16, 32};
    std::vector<std::vector<double>> operationCoeffs
    {     
        //vector<double> {40, 1, 1},
        //vector<double> {20, 1, 1},
        std::vector<double> {10, 1, 1},
        std::vector<double> {5, 1, 1},
        //vector<double> {3, 1, 1},
        //vector<double> {2, 1, 1},
        std::vector<double> {1, 1, 1},
        //vector<double> {1, 2, 1},
        //vector<double> {1, 2, 2},
    };

    std::vector<std::vector<double>> manualOperationCoeffs
    {
        std::vector<double> {1, 1, 1, 1, 1}
    };

    std::vector<std::vector<std::pair<int,int>>> agentNumbers
    {
        /*
        vector<pair<int,int>>
        {
            make_pair(50, 100),
            make_pair(50, 100),
            make_pair(50, 100)
        },
        */
        std::vector<std::pair<int,int>>
        {
            std::make_pair(100, 200),
            std::make_pair(100, 200),
            std::make_pair(100, 200)
        },
        
        
        /*
        vector<pair<int,int>>
        {
            make_pair(200, 400),
            make_pair(200, 400),
            make_pair(200, 400)
        },
        */
        /*
        vector<pair<int,int>>
        {
            make_pair(400, 800),
            make_pair(400, 800),
            make_pair(400, 800)
        }
        */
        /*
        vector<pair<int,int>>
        {
            make_pair(50, 60),
            make_pair(15, 20),
            make_pair(15, 20)
        },
        */
        
        /*
        vector<pair<int,int>>
        {
            make_pair(20, 40),
            make_pair(10, 20),
            make_pair(10, 20)
        },
        */
    };
    std::vector<int> jobNums {500};

    std::fstream file;
    std::vector<unsigned> seeds {694590411,709676904,724777594,739894479,754959835,770033789,785102292,800241863,815504173,830828699};
    
    FactoryFloorBenchmark ffb;
    SchedulerSettings* schs = new SchedulerSettings;
    schs->optimizationMethod = TxnSP::SolverType::SA;

    
    int c = 0;

    for(int jn : jobNums)
    {
        ffb.setJobNumber(jn);
        for(double ts : timeScales)
        {
            ffb.setTimeScale(ts);

            for(auto an : agentNumbers)
            {
                ffb.setAssemblyWorkerNumber(an[0].first, an[0].second);
                ffb.setTransporterNumber(an[1].first, an[1].second);
                ffb.setInspectorNumber(an[2].first, an[2].second);

                for(auto oc : operationCoeffs)
                {
                    ffb.setOperationCoefficients(oc[0], oc[1], oc[2]);

                    for(auto moc : manualOperationCoeffs)
                    {
                        ffb.setManualOperationCoefficients(moc[0], moc[1], moc[2], moc[3], moc[4]);

                        for(int tn : threadNumbers)
                        {                            
                            ffb.setThreadNumber(tn);
                            
                            schs->optimized = false;
                            ffb.setSchedulerSettings(schs);
                            
                            file.open("../data/results.csv", std::fstream::out | std::fstream::app);
                            
                            file << std::to_string(jn) << "," << std::to_string(ts) << "," << std::to_string(an[0].first) << "," << std::to_string(an[0].second) <<
                            "," << std::to_string(an[1].first) << "," << std::to_string(an[1].second) << "," << std::to_string(an[2].first)
                            << "," << std::to_string(an[2].second) << "," << std::to_string(oc[0]) << "," << std::to_string(oc[1]) << ","
                            << std::to_string(oc[2]) << "," << std::to_string(moc[0]) << "," << std::to_string(moc[1]) << "," << std::to_string(moc[2])
                            << "," << std::to_string(moc[3]) << "," << std::to_string(moc[4]) << "," << std::to_string(tn) << ",,,,,,";

                            std::cout << "---Working on no optimization---\n";
                            
                            for(int i = 0; i < 5; i++)
                            {
                                std::stringstream sstream;
                                sstream.setf(std::ios::fixed);
                                sstream.precision(2);
                                sstream << "_" << jn << "_" << ts << "_" << an[0].first << "_" << an[0].second
                                << "_" << an[1].first << "_" << an[1].second << "_" << an[2].first << "_" << an[2].second << "_" 
                                << oc[0] << "_" << oc[1] << "_" << oc[2] << "_" << tn << "_" << i << ".txt";

                                ffb.setSeed(seeds[i]);
                                ffb.keepStats("../data/stats/stats" + sstream.str());
                                ffb.saveJobs("../data/jobs/jobs" + sstream.str());
                                double res = ffb.StartBenchmark();
                                file << ",";
                                std::cout << "Run " + std::to_string(i) + " is completed " + std::to_string(res) + "\n";

                                if(res > -1)
                                {
                                    file << std::to_string(res);
                                }
                            }

                            file << "\n";
                            file.flush();
                            file.close();                            
                            
                            for(int bs : batchSizes)                            
                            {
                                ffb.setBatchSize(bs);

                                for(std::chrono::milliseconds timeout : timeouts)
                                {
                                    ffb.setTimeout(timeout);

                                    for(double t : temperatures)
                                    {
                                        schs->SA_MaxTemperature = t;

                                        for(auto ds : decrementSettings)
                                        {
                                            schs->SA_DecrementType = ds.first;
                                            schs->SA_DecrementParameter = ds.second;
                                            ffb.setSchedulerSettings(schs);

                                            
                                            file.open("../data/results.csv", std::fstream::out | std::fstream::app);

                                            file << std::to_string(jn) << "," << std::to_string(ts) << "," << std::to_string(an[0].first) << "," << std::to_string(an[0].second) <<
                                            "," << std::to_string(an[1].first) << "," << std::to_string(an[1].second) << "," << std::to_string(an[2].first)
                                            << "," << std::to_string(an[2].second) << "," << std::to_string(oc[0]) << "," << std::to_string(oc[1]) << ","
                                            << std::to_string(oc[2]) << "," << std::to_string(moc[0]) << "," << std::to_string(moc[1]) << "," << std::to_string(moc[2])
                                            << "," << std::to_string(moc[3]) << "," << std::to_string(moc[4]) << "," << std::to_string(tn) << "," <<
                                            std::to_string((double)timeout.count() / 1000000) << "," << std::to_string(bs) << "," << std::to_string(t) << "," << std::to_string(static_cast<int>(ds.first)) << ","
                                            << std::to_string(ds.second) << ",false";

                                            schs->optimized = true;
                                            schs->permuted = false;
                                            
                                            std::cout << "---Working on optimized---\n";

                                            for(int i = 0; i < 5; i++)
                                            {
                                                std::stringstream sstream;
                                                sstream.setf(std::ios::fixed);
                                                sstream.precision(2);
                                                sstream << "_" << jn << "_" << ts << "_" << an[0].first << "_" << an[0].second
                                                << "_" << an[1].first << "_" << an[1].second << "_" << an[2].first << "_" << an[2].second << "_" 
                                                << oc[0] << "_" << oc[1] << "_" << oc[2] << "_" << tn << "_" << i << ".txt";
                                                ffb.setSeed(seeds[i]);
                                                ffb.setSchedulerSettings(schs);
                                                ffb.setEstimatorFile("../data/stats/stats" + sstream.str());
                                                ffb.useExistingJobs("../data/jobs/jobs" + sstream.str());

                                                double res = ffb.StartBenchmark();
                                                file << ",";
                                                std::cout << "Run " + std::to_string(i) + " is completed " + std::to_string(res) + "\n";

                                                if(res > -1)
                                                {
                                                    file << std::to_string(res);
                                                }
                                            }
                                            
                                            file << "\n";
                                            file.flush();
                                            file.close();
                                            
                                            
                                            /*
                                            file.open("../data/results.csv", fstream::out | fstream::app);

                                            file << std::to_string(jn) << "," << std::to_string(ts) << "," << std::to_string(an[0].first) << "," << std::to_string(an[0].second) <<
                                            "," << std::to_string(an[1].first) << "," << std::to_string(an[1].second) << "," << std::to_string(an[2].first)
                                            << "," << std::to_string(an[2].second) << "," << std::to_string(oc[0]) << "," << std::to_string(oc[1]) << ","
                                            << std::to_string(oc[2]) << "," << std::to_string(moc[0]) << "," << std::to_string(moc[1]) << "," << std::to_string(moc[2])
                                            << "," << std::to_string(moc[3]) << "," << std::to_string(moc[4]) << "," << std::to_string(tn) << "," <<
                                            std::to_string(timeout.count() / 1000000) << "," << std::to_string(bs) << "," << std::to_string(t) << "," << std::to_string(static_cast<int>(ds.first)) << ","
                                            << std::to_string(ds.second) << ",true";
                                            
                                            schs->optimized = true;
                                            schs->permuted = true;

                                            cout << "---Working on permuted---\n";

                                            for(int i = 0; i < 5; i++)
                                            {
                                                stringstream sstream;
                                                sstream.setf(std::ios::fixed);
                                                sstream.precision(2);
                                                sstream << "_" << jn << "_" << ts << "_" << an[0].first << "_" << an[0].second
                                                << "_" << an[1].first << "_" << an[1].second << "_" << an[2].first << "_" << an[2].second << "_" 
                                                << oc[0] << "_" << oc[1] << "_" << oc[2] << "_" << tn << "_" << i << ".txt";
                                                ffb.setSeed(seeds[i]);                                  
                                                ffb.setSchedulerSettings(schs);
                                                ffb.setEstimatorFile("../data/stats/stats" + sstream.str());
                                                ffb.useExistingJobs("../data/jobs/jobs" + sstream.str());
                                                double res = ffb.StartBenchmark();
                                                file << ",";
                                                cout << "Run " + std::to_string(i) + " is completed " + std::to_string(res) + "\n";

                                                if(res > -1)
                                                {
                                                    file << std::to_string(res);
                                                }
                                            }

                                            file << "\n";
                                            file.flush();
                                            file.close();      
                                            */  
                                        }
                                    }
                                }
                            }
                        }
                    }
                }                 
            }
        }
    }
    
    
    /*
    FactoryFloorBenchmark ffb;
    //ffb.UseExistingJobs("/home/umut/jobs.csv");
    ffb.SetAssemblyWorkerNumber(20, 45);
    ffb.SetTransporterNumber(10, 20);
    ffb.SetInspectorNumber(10, 20);
    ffb.SetThreadNumber(4);
    ffb.SetTimeScale(0.1);
    ffb.SetTimeStep(500);
    ffb.SetBatchSize(30);
    ffb.SetJobNumber(100);
    ffb.UseExistingJobs("/home/umut/jobs.csv");
    
    SchedulerSettings* schs = new SchedulerSettings();

    schs->optimized = false;
    schs->optimizationMethod = TxnSP::SA;
    schs->SA_DecrementType = TxnSP::Linear;
    schs->SA_MaxTemperature = 500;
    schs->SA_DecrementParameter = 0.1;
    schs->permuted = true;

    ffb.SetSchedulerSettings(schs);
    ffb.SetEstimatorFile("/home/umut/stats.csv");


    //ffb.KeepStats("/home/umut/");
    //ffb.SaveJobs("/home/umut/");
    double res = ffb.StartBenchmark();
    
    cout << std::to_string(res);
    */

    
    /*
    queue<int> asd;
    asd.pop();

    MultiAgentModel model;
    model.addAgentTemplate<AgentTemplate1>(1, 15, 25, true);
    model.addAgentTemplate<AgentTemplate2>(2, 20, 40, true);
    //model.AddAgentTemplate<AgentTemplate1>(2,18,22, true);
    model.addAgentTemplate<AgentTemplate3>(3, 1, 1, true);
    model.addPlugin<Plugin1>(0, SHAREABLE);
    model.addPlugin<Plugin2>(1, NONSHAREABLE);
    model.allowPluginUse(1, 0);
    model.allowPluginUse(1, 1);
    model.allowPluginUse(2, 0);
    model.allowPluginUse(2, 1);
    model.setThreadNumber(4);
    model.addSupervisor(3, 1);
    model.addSupervisor(3, 2);
    model.setTrigger();
    model.setBatchSize(20);
    //auto init = make_unique<InitialTransaction>();
    

    TransactionFactory1 tfac;
    model.setTransactionFactory(&tfac);
    //model.KeepStatsFile("/home/umut/");
    
    Estimator1* est1 = new Estimator1();
    model.addEstimator(est1);
    //model.UseDefaultEstimator("/home/umut/stats.csv");

    SchedulerSettings* schs = new SchedulerSettings;
    schs->optimized = false;
    schs->optimizationMethod = TxnSP::SA;
    schs->SA_MaxTemperature = 100;
    schs->SA_DecrementType = TxnSP::Linear;
    schs->SA_DecrementParameter = 0.1;
    schs->permuted = true;
    model.addSchedulerSettings(schs);

    Driver drv;
    drv.startModel(model);
    auto deed = drv.getOutputParameters();
    */

    return 0;
}