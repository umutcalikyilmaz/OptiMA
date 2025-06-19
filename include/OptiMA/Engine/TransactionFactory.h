#pragma once
#include "OptiMA/Engine/IListener.h"
#include "OptiMA/TransactionModels/ITransaction.h"
#include "OptiMA/TransactionModels/TransactionResult.h"

namespace OptiMA
{
    class TransactionFactory
    {
    public:

        void initiate();

        void postProcess(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result);

    protected:

        virtual vector<unique_ptr<ITransaction>> generateInitialTransactions() = 0;

        virtual vector<unique_ptr<ITransaction>> generateTransactions(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result) = 0;        

    private:

        IListener* listener_;

        void insertListener(IListener* listener);

        friend class Driver;
    };
}