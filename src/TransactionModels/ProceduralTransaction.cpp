#include <TransactionModels/ProceduralTransaction.h>

namespace OptiMA
{
    ProceduralTransaction::ProceduralTransaction(Agent* owner, TransactionType type, int subType) : Transaction(owner, type, subType){ }

    ProceduralTransaction::ProceduralTransaction(Agent* owner, TransactionType type, int subType, set<int>& nonShareablePlugins)
    : Transaction(owner, type, subType, nonShareablePlugins) { }

    unique_ptr<ITransaction> ProceduralTransaction::Execute()
    {
        return make_unique<Process>(owner, Procedure());
    }
}