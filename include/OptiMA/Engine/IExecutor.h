#pragma once
#include "OptiMA/Engine/ExecutorState.h"

namespace OptiMA
{
    class IExecutor
    {
    public:
        
        virtual void assignTransaction(unique_ptr<ITransaction> txn, int index) = 0;

        virtual ~IExecutor() = default;
    };
}