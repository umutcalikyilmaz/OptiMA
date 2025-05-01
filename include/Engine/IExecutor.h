#pragma once
#include <TransactionModels/HaltProgram.h>

namespace OptiMA
{
    class IExecutor
    {
    public:
        
        virtual void AssignTransaction(unique_ptr<ITransaction> txn, int index) = 0;

        virtual ~IExecutor() = default;
    };
}