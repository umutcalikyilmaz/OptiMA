#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/Engine/Estimator.h"
#include "OptiMA/Engine/IListener.h"
#include "OptiMA/Engine/IScheduler.h"

namespace OptiMA
{
    class Listener : public IListener
    {
    public:

        Listener(IDriver* driver, AgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster);

        Listener(IDriver* driver, AgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster, Estimator* estimator, IScheduler* scheduler, bool trigger);

        Listener(IDriver* driver, AgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster, Estimator* estimator, IScheduler* scheduler, bool trigger,
        int batchSize);

        void sendTransaction(unique_ptr<ITransaction> txn) override;

        void trigger() override;

        TransactionQueue* getTransactionQueue();

        ~Listener();

    private:

        Estimator* estimator_;
        IScheduler* scheduler_;
        IDriver* driver_;
        AgentManager* amanager_;
        PluginManager* pmanager_;
        Postmaster* postmaster_;
        TransactionQueue* txnQueue_;
        mutex timerLock_;
        mutex triggerLock_;
        condition_variable deleteCondition_;
        condition_variable triggerCondition_;
        long transactionCount_;
        int batchSize_;
        int currentNum_;
        atomic_bool running_;
        atomic_bool triggerRunning_;
        atomic_bool triggered_;
        bool numCheck_;
        bool optimized_;
        bool initial_;

        void checkTrigger();
    };
}