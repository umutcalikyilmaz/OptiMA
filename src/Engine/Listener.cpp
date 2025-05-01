#include <Engine/Listener.h>

namespace OptiMA
{
    void Listener::checkTrigger()
    {
        while(running)
        {
            unique_lock<mutex> lock(triggerLock);
            triggerCondition.wait(lock, [this]
            { 
                return triggered.load(); 
            });

            triggered = false;
            txnQueue->Trigger();
        }
        
        timerRunning = false;
        deleteCondition.notify_one();
    }

    void Listener::MeasureTime()
    {
        while(running)
        {
            long now = chrono::steady_clock::now().time_since_epoch().count();

            if(timeStep <= now - lastSent)
            {
                currentNum = 0;
                lastSent = now;
                txnQueue->Trigger();
            }

            std::this_thread::sleep_for(chrono::nanoseconds(timeStep + lastSent - now));
        }
        
        timerRunning = false;
        deleteCondition.notify_one();
    }

    Listener::Listener() : optimized(false), timeCheck(false), numCheck(false), running(true),
    txnQueue(new TransactionQueue()) { }

    Listener::Listener(Estimator* estimator, IScheduler* scheduler, double timeStep) : estimator(estimator),
    scheduler(scheduler), optimized(true), timeStep((long)(timeStep * 1000000)), timeCheck(true), numCheck(false),
    running(true), timerRunning(true), txnQueue(new TransactionQueue())
    {
        lastSent = chrono::steady_clock::now().time_since_epoch().count();
        thread timer(&Listener::checkTrigger, this);
        timer.detach();
    }    

    Listener::Listener(Estimator* estimator, IScheduler* scheduler, int batchSize) : estimator(estimator),
    scheduler(scheduler), optimized(true), batchSize(batchSize), timeCheck(false), numCheck(true), running(true),
    txnQueue(new TransactionQueue()) { }

    Listener::Listener(Estimator* estimator, IScheduler* scheduler, double timeStep, int batchSize) : estimator(estimator),
    scheduler(scheduler), optimized(true), timeStep((long)(timeStep * 1000000)), batchSize(batchSize), timeCheck(true), numCheck(true),
    running(true), timerRunning(true), txnQueue(new TransactionQueue())
    {
        lastSent = chrono::steady_clock::now().time_since_epoch().count();
        thread timer(&Listener::checkTrigger, this);
        timer.detach();
    }

    void Listener::SendTransaction(unique_ptr<ITransaction> txn)
    {
        if(!running)
        {
            return;
        }

        if(optimized)
        {
            txn->SetLength(estimator->EstimateLength(*txn));
            txnQueue->SilentPush(move(txn));
            currentNum++;

            if(currentNum >= batchSize && numCheck)
            {
                currentNum = 0;
                txnQueue->Trigger();
            }
        }
        else
        {
            txnQueue->Push(move(txn));
        }
    }

    void Listener::trigger()
    {
        triggered = true;
        triggerCondition.notify_one();
    }

    TransactionQueue* Listener::GetTransactionQueue()
    {
        return txnQueue;
    }

    Listener::~Listener()
    {
        running = false;
        trigger();

        unique_lock<mutex> lock(timerLock);
        deleteCondition.wait(lock, [this]
        { 
            return !timerRunning.load(); 
        });
    }
}