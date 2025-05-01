#include <TransactionModels/TransactionResult.h>

namespace OptiMA
{
    TransactionResult::TransactionResult(TransactionStatus status) : status(status) { }

    TransactionResult::TransactionResult(TransactionStatus status, const char* errorMessage) : status(status),
    errorMessage(errorMessage) { }

    TransactionResult::TransactionResult(TransactionStatus status, shared_ptr<Memory> resultParameters) : status(status),
    resultParameters(resultParameters) { }
}