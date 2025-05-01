#pragma once
#include <TransactionModels/ProcessingTransaction.h>

namespace OptiMA
{
    class Begin final : public ProcessingTransaction
    {
        shared_ptr<Memory> initialParameters;

    public:
    
        Begin(Agent* owner, shared_ptr<Memory> initialParameters);

        unique_ptr<ITransaction> Processing() override;
    };
}