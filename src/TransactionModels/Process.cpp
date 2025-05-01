#include <TransactionModels/Process.h>

namespace OptiMA
{    
    Process::Process(Agent* owner, shared_ptr<TransactionResult> previousResult) : ProcessingTransaction(owner, PROCESS, 0),
    previousResult(previousResult) { }

    unique_ptr<ITransaction> Process::Processing()
    {
        return owner->Process(previousResult);
    }
}