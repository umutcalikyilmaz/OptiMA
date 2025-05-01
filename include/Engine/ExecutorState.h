#pragma once
#include <Shared/Types.h>
#include <TransactionModels/TransactionQueue.h>

namespace OptiMA
{
    struct alignas(64) ExecutorState
    {
        TransactionQueue* txnQueue;
        atomic_bool started;
        atomic_bool running;
        const bool optimized;
        map<int, map<int, map<int, double>>> totalTimes;
        map<int, map<int, map<int, int>>> counts;

        ExecutorState();

        ExecutorState(TransactionQueue* txnQueue);

        ~ExecutorState();
    };
}