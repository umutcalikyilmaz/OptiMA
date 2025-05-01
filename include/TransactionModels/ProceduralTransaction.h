#pragma once
#include <TransactionModels/Process.h>

namespace OptiMA
{
    class ProceduralTransaction : public Transaction
    {
    protected:

        ProceduralTransaction(Agent* owner, TransactionType type, int subType);

        ProceduralTransaction(Agent* owner, TransactionType type, int subType, set<int>& nonShareablePlugins);

    public:

        unique_ptr<ITransaction> Execute() override;

        virtual unique_ptr<TransactionResult> Procedure() = 0;

        virtual ~ProceduralTransaction() = default;
    };
}