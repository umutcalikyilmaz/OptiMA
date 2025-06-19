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
    IScheduler* scheduler, bool trigger) : driver_(driver), amanager_(amanager), pmanager_(pmanager), postmaster_(postmaster),
    estimator_(estimator), scheduler_(scheduler), optimized_(true), numCheck_(false), running_(true), triggerRunning_(trigger),
    txnQueue_(new TransactionQueue()), transactionCount_(0), currentNum_(0), initial_(true)
    {
        if(trigger)
        {
            thread triggerThread(&Listener::checkTrigger, this);
            triggerThread.detach();
        }        
    }    

    Listener::Listener(IDriver* driver, AgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster, Estimator* estimator,
    IScheduler* scheduler, bool trigger, int batchSize) : driver_(driver), amanager_(amanager), pmanager_(pmanager), postmaster_(postmaster),
    estimator_(estimator), scheduler_(scheduler), optimized_(true), batchSize_(batchSize), numCheck_(true), running_(true),
    triggerRunning_(trigger), txnQueue_(new TransactionQueue()), transactionCount_(0), currentNum_(0), initial_(true)
    {
        if(trigger)
        {
            thread triggerThread(&Listener::checkTrigger, this);
            triggerThread.detach();
        }
    }

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
        //txn->clear();

        if(optimized_)
        {
            txn->setLength(estimator_->estimateLength(*txn));
            txnQueue_->silentPush(move(txn));            

            if((currentNum_ >= batchSize_ && numCheck_) || initial_)
            {
                initial_ = false;
                currentNum_ = 0;
                txnQueue_->trigger();
            }
        }
        else
        {
            txnQueue_->push(move(txn));
        }

        currentNum_++;
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