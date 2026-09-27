#pragma once
#include "OptiMA/TransactionModels/TransactionQueue.h"

namespace OptiMA
{
    class IListener
    {
    public:

        class TransactionFactoryKey
        {
        private:

            TransactionFactoryKey() {}

            friend class TransactionFactory;
        };

        virtual void sendTransaction(TransactionFactoryKey, std::unique_ptr<ITransaction> txn) = 0;

        virtual ~IListener() = default;
    };
}


