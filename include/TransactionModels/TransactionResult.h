#pragma once
#include <Shared/Message.h>

namespace OptiMA
{
    struct TransactionResult : public Message
    {
        shared_ptr<Memory> resultParameters;
        const char* errorMessage;
        TransactionStatus status;

        TransactionResult(TransactionStatus status);

        TransactionResult(TransactionStatus status, const char* errorMessage);

        TransactionResult(TransactionStatus status, shared_ptr<Memory>  resultParameters);
    };
    
}