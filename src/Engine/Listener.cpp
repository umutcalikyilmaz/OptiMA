#include "OptiMA/Engine/Listener.h"

namespace OptiMA
{
    void Listener::checkTrigger()
    {
        while(running_)
        {
            std::unique_lock<std::mutex> lock(triggerLock_);
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

    void Listener::trigger()
    {
        triggered_ = true;
        triggerCondition_.notify_one();
    }

    Listener::Listener(DriverKey, IDriver* driver, AgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster)
        : driver_(driver),
          amanager_(amanager),
          pmanager_(pmanager),
          postmaster_(postmaster),
          optimized_(false),
          numCheck_(false),
          running_(true),
          txnQueue_(std::make_unique<TransactionQueue>()),
          transactionCount_(0), 
          currentNum_(0),
          initial_(true) { }    

    Listener::Listener(DriverKey, IDriver* driver, AgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster,
        Estimator* estimator, IScheduler* scheduler, int batchSize, std::chrono::milliseconds timeout)
        : driver_(driver),
          amanager_(amanager),
          pmanager_(pmanager),
          postmaster_(postmaster),
          estimator_(estimator),
          scheduler_(scheduler),
          optimized_(true),
          batchSize_(batchSize),
          numCheck_(true),
          running_(true),
          txnQueue_(std::make_unique<TransactionQueue>(batchSize, timeout)),
          transactionCount_(0),
          currentNum_(0),
          initial_(true) {  }

    void Listener::sendTransaction(TransactionFactoryKey, std::unique_ptr<ITransaction> txn)
    {
        if(!running_)
        {
            return;
        }        

        txn->setId(typename ITransaction::ListenerKey {}, transactionCount_++);  
        txn->setDriver(typename ITransaction::ListenerKey {}, driver_);      
        txn->setAgentManager(typename ITransaction::ListenerKey {}, amanager_);
        txn->setPostmaster(typename ITransaction::ListenerKey {}, postmaster_);
        txn->findNonShareable(typename ITransaction::ListenerKey {}, pmanager_);
        
        if(optimized_)
        {
            
            txn->setLength(typename ITransaction::ListenerKey {}, estimator_->estimateLength(*txn));
            txnQueue_->silentPush(move(txn));
        }
        else
        {
            txnQueue_->push(move(txn));
        }
    }

    TransactionQueue* Listener::getTransactionQueue(DriverKey)
    {
        return txnQueue_.get();
    }

    Listener::~Listener()
    {
        running_ = false;
        trigger();

        std::unique_lock<std::mutex> lock(triggerLock_);
        deleteCondition_.wait(lock, [this]
        { 
            return !triggerRunning_.load(); 
        });
    }
}