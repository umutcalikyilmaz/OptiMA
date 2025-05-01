#pragma once
#include <TransactionModels/Transaction.h>

namespace OptiMA
{
    class ProcessingTransaction : public Transaction
    {
    protected:

        ProcessingTransaction(Agent* owner, TransactionType type, int subType);

    public:        

        unique_ptr<ITransaction> Execute() override;

        virtual unique_ptr<ITransaction> Processing() = 0;

        virtual ~ProcessingTransaction() = default;
    };
}

