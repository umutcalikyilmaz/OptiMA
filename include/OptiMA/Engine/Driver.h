#pragma once
#include <condition_variable>
#include <fstream>
#include "OptiMA/Engine/AgentManager.h"
#include "OptiMA/Engine/DefaultEstimator.h"
#include "OptiMA/Engine/Executor.h"
#include "OptiMA/Engine/IDriver.h"
#include "OptiMA/Engine/Listener.h"
#include "OptiMA/Engine/MultiAgentModel.h"
#include "OptiMA/Engine/Scheduler.h"

namespace OptiMA
{
    class Driver : public IDriver
    {
    public:

        void startModel(MultiAgentModel& model);

        void haltProgram(std::shared_ptr<Memory> outputParameters) override;        

        std::shared_ptr<Memory> getOutputParameters();

    private:

        TransactionFactory* tfactory_;
        std::unique_ptr<PluginManager> pmanager_;
        std::unique_ptr<AgentManager> amanager_;
        std::unique_ptr<Executor> executor_;
        std::unique_ptr<Estimator> estimator_;
        std::unique_ptr<Listener> listener_;
        std::unique_ptr<Scheduler> scheduler_;        
        Postmaster* postmaster_;
        TransactionQueue* listenerQueue_;        
        std::unique_ptr<SchedulerSettings> schSettings_;
        std::shared_ptr<Memory> outputParameters_;
        std::mutex mainLock_;
        std::condition_variable cv_;
        std::string keepStatsFilePath_;
        long startingTime_;
        std::atomic_bool running_;
        bool keepStats_;
        bool settingsCreated_;

        Listener* createListener();

        void endProcesses();
    };
}