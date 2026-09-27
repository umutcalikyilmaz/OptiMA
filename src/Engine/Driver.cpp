#include "OptiMA/Engine/Driver.h"

namespace OptiMA
{
    void Driver::startModel(MultiAgentModel& model)
    {
        startingTime_ = std::chrono::steady_clock::now().time_since_epoch().count();      

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
            schSettings_ = std::make_unique<SchedulerSettings>(model.schSettings_);
        }
        else
        {
            schSettings_ = std::make_unique<SchedulerSettings>();
            settingsCreated_ = true;
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

        pmanager_ = std::make_unique<PluginManager>(typename PluginManager::DriverKey {}, model.instanceFactories_,
            model.pluginTypes_, model.pluginIds_, model.pluginAccesses_);

        amanager_ = std::make_unique<AgentManager>(typename AgentManager::DriverKey {}, model.agentFactories_,
            model.agentCoreIds_, model.initialNumbers_, model.maximumNumbers_, model.relationships_,
            model.communications_, model.pluginAccesses_, model.initialAgents_, pmanager_.get(), startingTime_);
            
        postmaster_ = amanager_->getPostmaster(typename AgentManager::DriverKey {});
        amanager_->startInitialAgents(typename AgentManager::DriverKey {});
        tfactory_ = model.tfactory_;
        
        std::set<int> nonShareablePlugins = pmanager_->getNonShareable();
        
        executor_ = std::make_unique<Executor>(typename Executor::DriverKey {}, this, tfactory_, pmanager_.get(),
            model.threadNumber_, nonShareablePlugins, schSettings_->optimized, keepStats_);
               
        if(schSettings_->optimized)
        {
            if(model.defaultEstimator_)
            {
                estimator_ = std::make_unique<DefaultEstimator>(model.defaultEstimatorFilePath_);
            }

            scheduler_ = std::make_unique<Scheduler>(typename Scheduler::DriverKey {}, schSettings_.get(),
                executor_.get(), nonShareablePlugins, model.threadNumber_);

            Estimator* estimatorPtr = model.defaultEstimator_ ? estimator_.get() : model.estimator_;
            listener_ = std::make_unique<Listener>(typename Listener::DriverKey {}, this, amanager_.get(),
                pmanager_.get(), postmaster_, estimatorPtr, scheduler_.get(), model.batchSize_, model.timeout_);

            listenerQueue_ = listener_->getTransactionQueue(typename Listener::DriverKey {});
            scheduler_->insertTransactionQueue(typename Scheduler::DriverKey {}, listenerQueue_);
        }
        else
        {
            listener_ = std::make_unique<Listener>(typename Listener::DriverKey {}, this, amanager_.get(),
                pmanager_.get(), postmaster_);
            listenerQueue_ = listener_->getTransactionQueue(typename Listener::DriverKey {});
            executor_->insertTransactionQueue(typename Executor::DriverKey {}, listenerQueue_);
        }

        tfactory_->insertListener(typename TransactionFactory::DriverKey {}, (IListener*)listener_.get());
        executor_->insertListener(typename Executor::DriverKey {}, (IListener*)listener_.get());
        executor_->start(typename Executor::DriverKey {});

        running_ = true;
        tfactory_->initiate(typename TransactionFactory::DriverKey {});

        if(schSettings_->optimized)
        {
            scheduler_->startScheduling(typename Scheduler::DriverKey {});
            endProcesses();
        }
        else
        {
            std::unique_lock<std::mutex> lock(mainLock_);
            cv_.wait(lock, [this]
            {
                return !running_.load();
            });

            endProcesses();
        }        
    }

    void Driver::haltProgram(std::shared_ptr<Memory> outputParameters)
    {
        this->outputParameters_ = outputParameters;
        
        if(schSettings_->optimized)
        {
            scheduler_ = nullptr;
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
        executor_->stop(typename Executor::DriverKey {});

        if(keepStats_)
        {
            auto statsMap = executor_->getStats(typename Executor::DriverKey {});
            std::fstream file;
            file.open(keepStatsFilePath_, std::fstream::out | std::fstream::trunc);

            for(auto p1 : statsMap)
            {
                for(auto p2 : p1.second)
                {
                    file << p1.first << "," << p2.first << "," << p2.second << "\n";
                }
            }

            file.close();
        }
        /*
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
        */
    }

    std::shared_ptr<Memory> Driver::getOutputParameters()
    {
        std::unique_lock<std::mutex> lock(mainLock_);
        cv_.wait(lock, [this] 
        {
            return !running_.load();
        });

        return outputParameters_;
    }
}