#pragma once
#include <Shared/Memory.h>

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

        int GetSenderId();

        int GetSenderType();

        int GetReceiverId();

        int GetReceiverType();

        friend class Postmaster;

    private:
        
        long timeStamp;
        int senderId;
        int senderType;
        int receiverId;
        int receiverType;
    };
}