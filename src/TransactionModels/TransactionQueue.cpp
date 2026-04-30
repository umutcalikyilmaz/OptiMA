#include "OptiMA/TransactionModels/TransactionQueue.h"

namespace OptiMA
{
    TransactionQueue::TransactionQueue() : triggered_(false), exit_(false), batchSize_(1), timeout_(1ms) { }

    TransactionQueue::TransactionQueue(int batchSize, chrono::milliseconds timeout) : triggered_(false), exit_(false), batchSize_(batchSize),
    timeout_(timeout) { }

    void TransactionQueue::silentPush(unique_ptr<ITransaction> txn)
    {
        lock_guard<mutex> lock(queueLock_);
        txnQueue_.push(move(txn));

        if((txnQueue_.size() >= batchSize_ && !triggered_) || initial_)
        {
            initial_ = false;
            trigger();
        }                    
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
        bool asd = cv_.wait_for(lock, timeout_, [this]
        {
            return triggered_.load() || exit_.load();
        });

        vector<unique_ptr<ITransaction>> res;

        if(exit_)
        {
            return res;
        }

        while(!txnQueue_.empty() && res.size() < batchSize_)
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