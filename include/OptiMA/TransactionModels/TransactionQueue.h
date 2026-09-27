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

        TransactionQueue(int batchSize, std::chrono::milliseconds timeout);

        void silentPush(std::unique_ptr<ITransaction> txn);

        void push(std::unique_ptr<ITransaction> txn);

        std::unique_ptr<ITransaction> pull();

        std::vector<std::unique_ptr<ITransaction>> pullAll();

        bool isEmpty();

        void trigger();

        void exit();

    private:

        std::queue<std::unique_ptr<ITransaction>> txnQueue_;
        std::mutex queueLock_;
        std::condition_variable cv_;
        const int batchSize_;
        const std::chrono::milliseconds timeout_;
        std::atomic_bool triggered_;
        std::atomic_bool exit_;
        bool initial_ = true;
    };
}