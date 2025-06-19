#include "OptiMA/Shared/Message.h"

namespace OptiMA
{
    Message::Message() { }

    Message::Message(string& prompt) : prompt(prompt) { }

    Message::Message(shared_ptr<Memory> parameters) : parameters(parameters) { }

    Message::Message(string& prompt, shared_ptr<Memory> parameters) : prompt(prompt), parameters(parameters) { }

    int Message::getSenderId()
    {
        return senderId_;
    }

    int Message::getSenderType()
    {
        return senderType_;
    }

    int Message::getReceiverId()
    {
        return receiverId_;
    }

    int Message::getReceiverType()
    {
        return receiverType_;
    }
}