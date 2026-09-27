#pragma once
#include "OptiMA/Engine/ExecutorState.h"

namespace OptiMA
{
    class IExecutor
    {
    public:

        class SchedulerKey
        {
        private:

            SchedulerKey() {};

            friend class Scheduler;
        };
        
        virtual void assignTransaction(SchedulerKey, std::unique_ptr<ITransaction> txn, int index) = 0;

        virtual ~IExecutor() = default;
    };
}