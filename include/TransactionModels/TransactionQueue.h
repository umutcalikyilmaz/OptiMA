#pragma once
#include <condition_variable>
#include <mutex>
#include <thread>
#include <Shared/Exceptions.h>
#include <TransactionModels/Transaction.h>

namespace OptiMA
{
    class TransactionQueue
    {
        queue<unique_ptr<ITransaction>> txnQueue;
        mutex queueLock;
        condition_variable cv;
        atomic_bool triggered;
        atomic_bool exit;

    public:

        TransactionQueue();

        void SilentPush(unique_ptr<ITransaction> txn);

        void Push(unique_ptr<ITransaction> txn);

        unique_ptr<ITransaction> Pull();

        vector<unique_ptr<ITransaction>> PullAll();

        bool isEmpty();

        void Trigger();

        void Exit();
    };
}