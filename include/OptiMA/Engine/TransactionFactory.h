#pragma once
#include "OptiMA/Engine/IListener.h"
#include "OptiMA/TransactionModels/ITransaction.h"
#include "OptiMA/TransactionModels/TransactionResult.h"

namespace OptiMA
{
    class TransactionFactory
    {
    public:

        class DriverKey
        {
        private:

            DriverKey() {}

            friend class Driver;
        };

        class ExecutorKey
        {
        private: 

            ExecutorKey() {}

            friend class Executor;
        };

        void insertListener(DriverKey, IListener* listener);

        void initiate(DriverKey);

        void postProcess(ExecutorKey, std::unique_ptr<ITransaction> txn,
            std::shared_ptr<TransactionResult> result);

    private:

        IListener* listener_;

        virtual std::vector<std::unique_ptr<ITransaction>> generateInitialTransactions() = 0;

        virtual std::vector<std::unique_ptr<ITransaction>> generateTransactions(std::unique_ptr<ITransaction> txn,
            std::shared_ptr<TransactionResult> result) = 0;
    };
}