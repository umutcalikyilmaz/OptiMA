#pragma once
#include <condition_variable>
#include <fstream>
#include <Engine/AgentManager.h>
#include <Engine/DefaultEstimator.h>
#include <Engine/Executor.h>
#include <Engine/IDriver.h>
#include <Engine/Listener.h>
#include <Engine/MultiAgentModel.h>
#include <Engine/Scheduler.h>

namespace OptiMA
{
    class Driver : public IDriver
    {
        PluginManager* pmanager;
        Executor* executor;
        Scheduler* scheduler;
        Estimator* estimator;
        Listener* listener;
        TransactionQueue* listenerQueue;
        AgentManager* amanager;
        Postmaster* postmaster;
        SchedulerSettings* schSettings;
        shared_ptr<Memory> outputParameters;
        long startingTime;
        atomic_bool running;
        mutex mainLock;
        condition_variable cv;
        string keepStatsFilePath;
        bool keepStats;
        

        Listener* CreateListener();

        void EndProcesses();
        
    public:

        void StartModel(MultiAgentModel& model);

        void HaltProgram(shared_ptr<Memory> outputParameters) override;        

        shared_ptr<Memory> GetOutputParameters();
    };
}