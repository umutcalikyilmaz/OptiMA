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

        void haltProgram(shared_ptr<Memory> outputParameters) override;        

        shared_ptr<Memory> getOutputParameters();

    private:

        TransactionFactory* tfactory_;
        PluginManager* pmanager_;
        Executor* executor_;
        Scheduler* scheduler_;
        Estimator* estimator_;
        Listener* listener_;
        TransactionQueue* listenerQueue_;
        AgentManager* amanager_;
        Postmaster* postmaster_;
        SchedulerSettings* schSettings_;
        shared_ptr<Memory> outputParameters_;
        mutex mainLock_;
        condition_variable cv_;
        string keepStatsFilePath_;
        long startingTime_;
        atomic_bool running_;
        bool keepStats_;
        bool trigger_;
        bool settingsCreated_;

        Listener* createListener();

        void endProcesses();
    };
}