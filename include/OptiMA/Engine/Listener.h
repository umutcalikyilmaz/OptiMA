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

        class DriverKey
        {
        private:

            DriverKey() {}

            friend class Driver;
        };

        Listener(DriverKey, IDriver* driver, AgentManager* amanager, PluginManager* pmanager,
            Postmaster* postmaster);

        Listener(DriverKey, IDriver* driver, AgentManager* amanager, PluginManager* pmanager,
            Postmaster* postmaster, Estimator* estimator, IScheduler* scheduler, int batchSize,
            std::chrono::milliseconds timeout);

        void sendTransaction(TransactionFactoryKey, std::unique_ptr<ITransaction> txn) override;        

        TransactionQueue* getTransactionQueue(DriverKey);

        ~Listener();

    private:

        Estimator* estimator_;
        IScheduler* scheduler_;
        IDriver* driver_;
        AgentManager* amanager_;
        PluginManager* pmanager_;
        Postmaster* postmaster_;
        std::unique_ptr<TransactionQueue> txnQueue_;
        std::mutex timerLock_;
        std::mutex triggerLock_;
        std::mutex queueLock_;
        std::condition_variable deleteCondition_;
        std::condition_variable triggerCondition_;
        long transactionCount_;
        int batchSize_;
        int currentNum_;
        std::atomic_bool running_;
        std::atomic_bool triggerRunning_;
        std::atomic_bool triggered_;
        bool numCheck_;
        bool optimized_;
        bool initial_;

        void checkTrigger();

        void trigger();
    };
}