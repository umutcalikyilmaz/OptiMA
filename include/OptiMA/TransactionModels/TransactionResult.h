#pragma once
#include "OptiMA/Shared/Message.h"

namespace OptiMA
{
    struct TransactionResult : public Message
    {
        std::shared_ptr<Memory> resultParameters;
        const char* errorMessage;
        TransactionStatus status;

        TransactionResult(TransactionStatus status);

        TransactionResult(TransactionStatus status, const char* errorMessage);

        TransactionResult(TransactionStatus status, std::shared_ptr<Memory> resultParameters);
    };
    
}