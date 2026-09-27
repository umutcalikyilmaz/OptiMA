#include "OptiMA/TransactionModels/TransactionQueue.h"

namespace OptiMA
{
    TransactionQueue::TransactionQueue()
        : triggered_(false),
          exit_(false),
          batchSize_(1),
          timeout_(std::chrono::milliseconds(1)) { }

    TransactionQueue::TransactionQueue(int batchSize, std::chrono::milliseconds timeout)
        : triggered_(false),
          exit_(false),
          batchSize_(batchSize),
          timeout_(timeout) { }

    void TransactionQueue::silentPush(std::unique_ptr<ITransaction> txn)
    {
        std::lock_guard<std::mutex> lock(queueLock_);
        txnQueue_.push(move(txn));

        if((txnQueue_.size() >= batchSize_ && !triggered_) || initial_)
        {
            initial_ = false;
            trigger();
        }                    
    }

    void TransactionQueue::push(std::unique_ptr<ITransaction> txn)
    {      
        std::lock_guard<std::mutex> lock(queueLock_);
        txnQueue_.push(move(txn));
        cv_.notify_one();
    }

    std::unique_ptr<ITransaction> TransactionQueue::pull()
    {        
        std::unique_lock<std::mutex> lock(queueLock_);
        cv_.wait(lock, [this] 
        {
            return !txnQueue_.empty() || exit_.load();
        });

        if(exit_)
        {
            return nullptr;            
        }

        std::unique_ptr<ITransaction> res = move(txnQueue_.front());
        txnQueue_.pop();
        return res;
    }
    
    std::vector<std::unique_ptr<ITransaction>> TransactionQueue::pullAll()
    {
        std::unique_lock<std::mutex> lock(queueLock_);
        bool asd = cv_.wait_for(lock, timeout_, [this]
        {
            return triggered_.load() || exit_.load();
        });

        std::vector<std::unique_ptr<ITransaction>> res;

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
        std::lock_guard<std::mutex> lock(queueLock_);
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