#pragma once
#include <chrono>
#include <thread>
#include <Engine/Estimator.h>
#include <Engine/IListener.h>
#include <Engine/IScheduler.h>

namespace OptiMA
{
    class Listener : public IListener
    {
        Estimator* estimator;
        IScheduler* scheduler;
        TransactionQueue* txnQueue;
        mutex timerLock;
        mutex triggerLock;
        condition_variable deleteCondition;
        condition_variable triggerCondition;
        long timeStep;
        long lastSent;
        int batchSize;
        int currentNum;
        atomic_bool running;
        atomic_bool timerRunning;
        atomic_bool triggered;
        bool timeCheck;
        bool numCheck;
        bool optimized;

        void checkTrigger();

        void MeasureTime();
        
    public:

        Listener();

        Listener(Estimator* estimator, IScheduler* scheduler, double timeStep);

        Listener(Estimator* estimator, IScheduler* scheduler, int batchSize);

        Listener(Estimator* estimator, IScheduler* scheduler, double timeStep, int batchSize);

        void SendTransaction(unique_ptr<ITransaction> txn) override;

        void trigger() override;

        TransactionQueue* GetTransactionQueue();

        ~Listener();
    };
}