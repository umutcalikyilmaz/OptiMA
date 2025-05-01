#include <TransactionModels/TransactionQueue.h>

namespace OptiMA
{
    TransactionQueue::TransactionQueue() : triggered(false), exit(false) { }

    void TransactionQueue::SilentPush(unique_ptr<ITransaction> txn)
    {
        lock_guard<mutex> lock(queueLock);
        txnQueue.push(move(txn));
    }

    void TransactionQueue::Push(unique_ptr<ITransaction> txn)
    {      
        lock_guard<mutex> lock(queueLock);
        txnQueue.push(move(txn));
        cv.notify_one();
    }

    unique_ptr<ITransaction> TransactionQueue::Pull()
    {        
        unique_lock<mutex> lock(queueLock);
        cv.wait(lock, [this] 
        {
            return !txnQueue.empty() || exit.load();
        });

        if(exit)
        {
            return nullptr;            
        }

        unique_ptr<ITransaction> res = move(txnQueue.front());
        txnQueue.pop();
        return res;
    }
    
    vector<unique_ptr<ITransaction>> TransactionQueue::PullAll()
    {
        unique_lock<mutex> lock(queueLock);
        cv.wait(lock, [this]
        { 
            return triggered.load() || exit.load(); 
        });

        vector<unique_ptr<ITransaction>> res;

        if(exit)
        {
            return res;
        }

        while(!txnQueue.empty())
        {
            res.push_back(move(txnQueue.front()));
            txnQueue.pop();
        }

        triggered = false;
        return res;
    }

    bool TransactionQueue::isEmpty()
    {
        lock_guard<mutex> lock(queueLock);
        return txnQueue.empty();
    }

    void TransactionQueue::Trigger()
    {
        triggered = true;
        cv.notify_one();
    }

    void TransactionQueue::Exit()
    {
        exit = true;
        cv.notify_all();
    }
}