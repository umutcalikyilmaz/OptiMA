#include "OptiMA/Engine/TransactionFactory.h"

namespace OptiMA
{
    void TransactionFactory::insertListener(DriverKey, IListener* listener)
    {
        listener_ = listener;
    }

    void TransactionFactory::initiate(DriverKey)
    {
        std::vector<std::unique_ptr<ITransaction>> txns = generateInitialTransactions();
        int c = txns.size();

        for(int i = 0; i < c; i++)
        {
            listener_->sendTransaction(typename IListener::TransactionFactoryKey {}, std::move(txns[i]));
        } 
    }

    void TransactionFactory::postProcess(ExecutorKey, std::unique_ptr<ITransaction> txn,
        std::shared_ptr<TransactionResult> result)
    {
        std::vector<std::unique_ptr<ITransaction>> txns = generateTransactions(move(txn), result);
        int c = txns.size();

        for(int i = 0; i < c; i++)
        {
            listener_->sendTransaction(typename IListener::TransactionFactoryKey {},std::move(txns[i]));
        } 
    }
}