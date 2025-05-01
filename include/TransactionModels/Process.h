#pragma once
#include <TransactionModels/ProcessingTransaction.h>

namespace OptiMA
{
    class Process final : public ProcessingTransaction
    {        
        shared_ptr<TransactionResult> previousResult;

    public:
    
        Process(Agent* owner, shared_ptr<TransactionResult> previousResult);        

        unique_ptr<ITransaction> Processing() override;
    };
}