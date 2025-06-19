#pragma once
#include "OptiMA/Shared/Memory.h"

namespace OptiMA
{
    struct Message
    {
        shared_ptr<Memory> parameters;
        string prompt;

        Message();

        Message(string& prompt);

        Message(shared_ptr<Memory> parameters);

        Message(string& prompt, shared_ptr<Memory> parameters);

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