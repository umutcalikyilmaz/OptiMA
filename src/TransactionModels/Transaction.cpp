#include <TransactionModels/Transaction.h>

namespace OptiMA
{
    void Transaction::Abort()
    {
        throw UserAbortException((char*)"Aborted by the agent");
    }

    Transaction::Transaction(Agent* owner, TransactionType type, int subType) : owner(owner), type(type), subType(subType) { }

    Transaction::Transaction(Agent* owner, TransactionType type, shared_ptr<Memory> outputParameters) : owner(owner),
    type(type), outputParameters(outputParameters) { }

    Transaction::Transaction(Agent* owner, TransactionType type, int subType, const set<int>& nonShareablePlugins) : owner(owner),
    type(type), subType(subType), nonShareablePlugins(nonShareablePlugins) { }

    const set<int>& Transaction::GetNonShareblePlugins()
    {
        return nonShareablePlugins;
    }

    TransactionType Transaction::GetTransactionType() const
    {
        return type;
    }

    int Transaction::GetAgentType() const
    {
        return owner->GetAgentType();
    }

    shared_ptr<Memory> Transaction::GetOutputParameters()
    {
        if(type != HALTPROGRAM)
        {
            return nullptr;
        }

        return outputParameters;
    }

    void Transaction::SetLength(double length)
    {
        this->length = length;
    }

    double Transaction::GetLength()
    {
        return length;
    }

    int Transaction::GetSubType() const
    {
        return subType;
    }
}