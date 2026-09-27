#pragma once
#include "OptiMA/Shared/Memory.h"

namespace OptiMA
{
    struct Message
    {
        std::shared_ptr<Memory> parameters;
        std::string prompt;

        Message();

        Message(const std::string& prompt);

        Message(std::shared_ptr<Memory> parameters);

        Message(const std::string& prompt, std::shared_ptr<Memory> parameters);

        int getSenderId();

        int getSenderType();

        int getReceiverId();

        int getReceiverType();

        friend class Postmaster;

    private:
        
        long timeStamp_;
        int senderId_;
        int senderType_;
        int receiverId_;
        int receiverType_;
    };
}