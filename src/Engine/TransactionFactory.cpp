#include "OptiMA/Engine/TransactionFactory.h"

namespace OptiMA
{
    void TransactionFactory::initiate()
    {
        vector<unique_ptr<ITransaction>> txns = generateInitialTransactions();
        int c = txns.size();

        for(int i = 0; i < c; i++)
        {
            listener_->sendTransaction(move(txns[i]));
        } 
    }

    void TransactionFactory::postProcess(unique_ptr<ITransaction> txn, shared_ptr<TransactionResult> result)
    {
        vector<unique_ptr<ITransaction>> txns = generateTransactions(move(txn), result);
        int c = txns.size();

        for(int i = 0; i < c; i++)
        {
            listener_->sendTransaction(move(txns[i]));
        } 
    }

    void TransactionFactory::insertListener(IListener* listener)
    {
        listener_ = listener;
    }
}