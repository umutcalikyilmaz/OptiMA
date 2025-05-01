#include <Engine/Driver.h>

namespace OptiMA
{
    void Driver::StartModel(MultiAgentModel& model)
    {
        startingTime = chrono::steady_clock::now().time_since_epoch().count();      

        if(!model.initialAgentsAdded)
        {
            throw InvalidModelParameterException((char*)"None of the agents are allowed to start at in the beginning");
        }

        if(model.schedulerSettingsAdded)
        {
            schSettings = model.schSettings;
        }
        else
        {
            schSettings = new SchedulerSettings();
            schSettings->optimized = false;
        }

        if(!model.estimatorAdded && schSettings->optimized)
        {
            throw InvalidModelParameterException((char*)"Optimized model cannot be started unless an estimator is added");
        }

        keepStats = model.keepStats;
        
        if(keepStats)
        {
            keepStatsFilePath = model.keepStatsFilePath;
        }

        pmanager = new PluginManager(model.instanceFactories, model.pluginTypes, model.pluginIds, model.pluginAccesses);
        
        set<int> nonShareablePlugins = pmanager->GetNonShareable();
        
        executor = new Executor(this, pmanager, model.threadNumber, nonShareablePlugins, model.haltingAgents,
        schSettings->optimized, keepStats);

        if(model.defaultEstimator)
        {
            estimator = new DefaultEstimator(model.defaultEstimatorFilePath);
        }
        else
        {
            estimator = model.estimator;
        }        

        if(schSettings->optimized)
        {
            if(model.timeCheck)
            {
                if(model.numCheck)
                {
                    listener = new Listener(estimator, scheduler, model.timeStep, model.batchSize);
                }
                else
                {
                    listener = new Listener(estimator, scheduler, model.timeStep);
                }
            }
            else
            {
                if(model.numCheck)
                {
                    listener = new Listener(estimator, scheduler, model.batchSize);
                }
                else
                {
                    listener = new Listener(estimator, scheduler, 5);
                }
            }

            listenerQueue = listener->GetTransactionQueue();
            scheduler = new Scheduler(schSettings, executor, nonShareablePlugins, model.threadNumber);
            scheduler->InsertTransactionQueue(listenerQueue);
        }
        else
        {
            listener = new Listener();
            listenerQueue = listener->GetTransactionQueue();
            executor->InsertTransactionQueue(listenerQueue);
        }

        executor->InsertListener((IListener*)listener);
        executor->Start();

        amanager = new AgentManager(model.coreFactories, model.agentCoreIds, model.initialNumbers, model.maximumNumbers,
        model.relationships, model.communications, model.pluginAccesses, model.initialAgents, model.initialParameters,
        pmanager, listener, startingTime);

        amanager->StartInitialAgents();
        running = true;

        if(schSettings->optimized)
        {
            scheduler->StartScheduling();
            EndProcesses();
        }
        else
        {
            unique_lock<mutex> lock(mainLock);
            cv.wait(lock, [this]
            {
                return !running.load();
            });

            EndProcesses();
        }        
    }

    void Driver::HaltProgram(shared_ptr<Memory> outputParameters)
    {
        this->outputParameters = outputParameters;
        
        if(schSettings->optimized)
        {
            delete scheduler;
            running = false;
            cv.notify_one();
        }
        else
        {
            this->outputParameters = outputParameters;
            running = false;
            cv.notify_one();
        }        
    }

    void Driver::EndProcesses()
    {
        listenerQueue->Exit();
        executor->Stop();

        if(keepStats)
        {
            auto statsMap = executor->GetStats();
            fstream file;
            file.open(keepStatsFilePath, fstream::out | fstream::trunc);

            for(auto p1 : statsMap)
            {
                for(auto p2 : p1.second)
                {
                    for(auto p3 : p2.second)
                    {
                        file << p1.first << "," << p2.first << "," << p3.first << "," << p3.second << "\n";
                    }
                }
            }

            file.close();
        }
        
        if(schSettings->optimized)
        {            
            delete executor;
            delete listener;
            delete estimator;
        }
        else
        {
            delete executor;
            delete listener;
        }
        
        delete listenerQueue;
        delete amanager;
        delete pmanager;
    }

    shared_ptr<Memory> Driver::GetOutputParameters()
    {
        unique_lock<mutex> lock(mainLock);
        cv.wait(lock, [this] 
        {
            return !running.load();
        });

        return outputParameters;
    }
}