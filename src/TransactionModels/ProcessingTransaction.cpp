#include <TransactionModels/ProcessingTransaction.h>

namespace OptiMA
{
    ProcessingTransaction::ProcessingTransaction(Agent* owner, TransactionType type, int subType) : Transaction(owner, type, subType) { }
    
    unique_ptr<ITransaction> ProcessingTransaction::Execute()
    {
        return Processing();
    }
}