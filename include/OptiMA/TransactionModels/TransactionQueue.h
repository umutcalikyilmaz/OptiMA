#pragma once
#include <condition_variable>
#include <mutex>
#include <thread>
#include "OptiMA/Shared/Exceptions.h"
#include "OptiMA/TransactionModels/ITransaction.h"

namespace OptiMA
{
    class TransactionQueue
    {
    public:

        TransactionQueue();

        TransactionQueue(int batchSize, chrono::milliseconds timeout);

        void silentPush(unique_ptr<ITransaction> txn);

        void push(unique_ptr<ITransaction> txn);

        unique_ptr<ITransaction> pull();

        vector<unique_ptr<ITransaction>> pullAll();

        bool isEmpty();

        void trigger();

        void exit();

    private:

        queue<unique_ptr<ITransaction>> txnQueue_;
        mutex queueLock_;
        condition_variable cv_;
        const int batchSize_;
        const chrono::milliseconds timeout_;
        atomic_bool triggered_;
        atomic_bool exit_;
        bool initial_ = true;
    };
}