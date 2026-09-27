#pragma once
#include "OptiMA/Shared/Types.h"
#include "OptiMA/TransactionModels/TransactionQueue.h"

namespace OptiMA
{
    struct alignas(64) ExecutorState
    {
        TransactionQueue* txnQueue;
        std::atomic_bool started;
        std::atomic_bool running;
        std::map<int, std::map<int, double>> totalTimes;
        std::map<int, std::map<int, int>> counts;

        ExecutorState(TransactionQueue* txnQueue);
    };
}