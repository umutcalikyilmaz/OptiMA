#pragma once
#include <TransactionModels/Transaction.h>

namespace OptiMA
{
    class HaltProgram final : public Transaction
    {
    public:
    
        HaltProgram(Agent* owner, shared_ptr<Memory> outputParameters) : Transaction(owner, HALTPROGRAM,
        outputParameters) { }
        
        unique_ptr<ITransaction> Execute() override { return nullptr; }

        ~HaltProgram() = default;
    };
}