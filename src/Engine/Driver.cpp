#include "OptiMA/Engine/Driver.h"

namespace OptiMA
{
    void Driver::startModel(MultiAgentModel& model)
    {
        startingTime_ = chrono::steady_clock::now().time_since_epoch().count();      

        if(!model.tfactorySet_)
        {
            throw InvalidModelParameterException("Transaction factory is not provided by the user");
        }

        if(!model.initialAgentsAdded_)
        {
            throw InvalidModelParameterException("None of the agents are allowed to start at in the beginning");
        }

        if(model.schedulerSettingsAdded_)
        {
            settingsCreated_ = false;
            schSettings_ = model.schSettings_;
            trigger_ = model.trigger_;
        }
        else
        {
            settingsCreated_ = true;
            schSettings_ = new SchedulerSettings();
            schSettings_->optimized = false;
        }

        if(!model.estimatorAdded_ && schSettings_->optimized)
        {
            throw InvalidModelParameterException("Optimized model cannot be started unless an estimator is added");
        }

        keepStats_ = model.keepStats_;
        
        if(keepStats_)
        {
            keepStatsFilePath_ = model.keepStatsFilePath_;
        }

        pmanager_ = new PluginManager(model.instanceFactories_, model.pluginTypes_, model.pluginIds_, model.pluginAccesses_);

        amanager_ = new AgentManager(model.agentFactories_, model.agentCoreIds_, model.initialNumbers_, model.maximumNumbers_,
        model.relationships_, model.communications_, model.pluginAccesses_, model.initialAgents_, pmanager_, startingTime_);
        postmaster_ = amanager_->getPostmaster();

        amanager_->startInitialAgents();

        tfactory_ = model.tfactory_;
        
        set<int> nonShareablePlugins = pmanager_->getNonShareable();
        
        executor_ = new Executor(this, tfactory_, pmanager_, model.threadNumber_, nonShareablePlugins, schSettings_->optimized,trigger_,
        keepStats_);

        if(model.defaultEstimator_)
        {
            estimator_ = new DefaultEstimator(model.defaultEstimatorFilePath_);
        }
        else
        {
            estimator_ = model.estimator_;
        }        

        if(schSettings_->optimized)
        {
            if(model.numCheck_)
            {
                listener_ = new Listener(this, amanager_, pmanager_, postmaster_, estimator_, scheduler_, trigger_, model.batchSize_);
            }
            else
            {
                listener_ = new Listener(this, amanager_, pmanager_, postmaster_, estimator_, scheduler_, trigger_);
            }

            listenerQueue_ = listener_->getTransactionQueue();
            scheduler_ = new Scheduler(schSettings_, executor_, nonShareablePlugins, model.threadNumber_);
            scheduler_->insertTransactionQueue(listenerQueue_);
        }
        else
        {
            listener_ = new Listener(this, amanager_, pmanager_, postmaster_);
            listenerQueue_ = listener_->getTransactionQueue();
            executor_->insertTransactionQueue(listenerQueue_);
        }

        tfactory_->insertListener((IListener*)listener_);
        executor_->insertListener((IListener*)listener_);
        executor_->start();

        running_ = true;
        tfactory_->initiate();

        if(schSettings_->optimized)
        {
            scheduler_->startScheduling();
            endProcesses();
        }
        else
        {
            unique_lock<mutex> lock(mainLock_);
            cv_.wait(lock, [this]
            {
                return !running_.load();
            });

            endProcesses();
        }        
    }

    void Driver::haltProgram(shared_ptr<Memory> outputParameters)
    {
        this->outputParameters_ = outputParameters;
        
        if(schSettings_->optimized)
        {
            delete scheduler_;
            running_ = false;
            cv_.notify_one();
        }
        else
        {
            this->outputParameters_ = outputParameters;
            running_ = false;
            cv_.notify_one();
        }        
    }

    void Driver::endProcesses()
    {
        listenerQueue_->exit();
        executor_->stop();

        if(keepStats_)
        {
            auto statsMap = executor_->getStats();
            fstream file;
            file.open(keepStatsFilePath_, fstream::out | fstream::trunc);

            for(auto p1 : statsMap)
            {
                for(auto p2 : p1.second)
                {
                    file << p1.first << "," << p2.first << "," << p2.second << "\n";
                }
            }

            file.close();
        }

        delete executor_;
        delete listener_;
        
        if(schSettings_->optimized)
        {
            delete estimator_;
        }
        
        delete listenerQueue_;
        delete amanager_;
        delete pmanager_;

        if(settingsCreated_)
        {
            delete schSettings_;
        }
    }

    shared_ptr<Memory> Driver::getOutputParameters()
    {
        unique_lock<mutex> lock(mainLock_);
        cv_.wait(lock, [this] 
        {
            return !running_.load();
        });

        return outputParameters_;
    }
}