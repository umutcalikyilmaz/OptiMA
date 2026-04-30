#include "OptiMA/Engine/Listener.h"

namespace OptiMA
{
    void Listener::checkTrigger()
    {
        while(running_)
        {
            unique_lock<mutex> lock(triggerLock_);
            triggerCondition_.wait(lock, [this]
            { 
                return triggered_.load(); 
            });

            triggered_ = false;
            txnQueue_->trigger();
        }
        
        triggerRunning_ = false;
        deleteCondition_.notify_one();
    }

    Listener::Listener(IDriver* driver, AgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster) : driver_(driver),
    amanager_(amanager), pmanager_(pmanager), postmaster_(postmaster), optimized_(false), numCheck_(false), running_(true),
    txnQueue_(new TransactionQueue()), transactionCount_(0), currentNum_(0), initial_(true) { }    

    Listener::Listener(IDriver* driver, AgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster, Estimator* estimator,
    IScheduler* scheduler, int batchSize, chrono::milliseconds timeout) : driver_(driver), amanager_(amanager), pmanager_(pmanager),
    postmaster_(postmaster), estimator_(estimator), scheduler_(scheduler), optimized_(true), batchSize_(batchSize), numCheck_(true),
    running_(true), txnQueue_(new TransactionQueue(batchSize, timeout)), transactionCount_(0), currentNum_(0), initial_(true) {  }

    void Listener::sendTransaction(unique_ptr<ITransaction> txn)
    {
        if(!running_)
        {
            return;
        }        

        txn->setId(transactionCount_++);  
        txn->setDriver(driver_);      
        txn->setAgentManager(amanager_);
        txn->setPostMaster(postmaster_);
        txn->findNonShareable(pmanager_);
        
        if(optimized_)
        {
            
            txn->setLength(estimator_->estimateLength(*txn));
            txnQueue_->silentPush(move(txn));
        }
        else
        {
            txnQueue_->push(move(txn));
        }
    }

    void Listener::trigger()
    {
        triggered_ = true;
        triggerCondition_.notify_one();
    }

    TransactionQueue* Listener::getTransactionQueue()
    {
        return txnQueue_;
    }

    Listener::~Listener()
    {
        running_ = false;
        trigger();

        unique_lock<mutex> lock(triggerLock_);
        deleteCondition_.wait(lock, [this]
        { 
            return !triggerRunning_.load(); 
        });
    }
}