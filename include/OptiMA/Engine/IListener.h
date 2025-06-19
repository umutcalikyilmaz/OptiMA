#pragma once
#include "OptiMA/TransactionModels/TransactionQueue.h"

namespace OptiMA
{
    class IListener
    {
    public:

        virtual void sendTransaction(unique_ptr<ITransaction> txn) = 0;

        virtual void trigger() = 0;

        virtual ~IListener() = default;
    };
}


