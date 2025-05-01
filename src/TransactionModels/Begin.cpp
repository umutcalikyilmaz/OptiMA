#include <TransactionModels/Begin.h>

namespace OptiMA
{
    Begin::Begin(Agent* owner, shared_ptr<Memory> initialParameters) : ProcessingTransaction(owner, BEGIN, 0), initialParameters(initialParameters) { }

    unique_ptr<ITransaction> Begin::Processing()
    {
        try
        {
            return owner->BeginProcess(initialParameters);
        }
        catch(exception e)
        {
            return nullptr;
        }
        
    }
}