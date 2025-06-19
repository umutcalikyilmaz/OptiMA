#include "OptiMA/TransactionModels/TransactionQueue.h"

namespace OptiMA
{
    TransactionQueue::TransactionQueue() : triggered_(false), exit_(false) { }

    void TransactionQueue::silentPush(unique_ptr<ITransaction> txn)
    {
        lock_guard<mutex> lock(queueLock_);
        txnQueue_.push(move(txn));
    }

    void TransactionQueue::push(unique_ptr<ITransaction> txn)
    {      
        lock_guard<mutex> lock(queueLock_);
        txnQueue_.push(move(txn));
        cv_.notify_one();
    }

    unique_ptr<ITransaction> TransactionQueue::pull()
    {        
        unique_lock<mutex> lock(queueLock_);
        cv_.wait(lock, [this] 
        {
            return !txnQueue_.empty() || exit_.load();
        });

        if(exit_)
        {
            return nullptr;            
        }

        unique_ptr<ITransaction> res = move(txnQueue_.front());
        txnQueue_.pop();
        return res;
    }
    
    vector<unique_ptr<ITransaction>> TransactionQueue::pullAll()
    {
        unique_lock<mutex> lock(queueLock_);
        cv_.wait(lock, [this]
        { 
            return triggered_.load() || exit_.load(); 
        });

        vector<unique_ptr<ITransaction>> res;

        if(exit_)
        {
            return res;
        }

        while(!txnQueue_.empty())
        {
            res.push_back(move(txnQueue_.front()));
            txnQueue_.pop();
        }

        triggered_ = false;
        return res;
    }

    bool TransactionQueue::isEmpty()
    {
        lock_guard<mutex> lock(queueLock_);
        return txnQueue_.empty();
    }

    void TransactionQueue::trigger()
    {
        triggered_ = true;
        cv_.notify_one();
    }

    void TransactionQueue::exit()
    {
        exit_ = true;
        cv_.notify_all();
    }
}