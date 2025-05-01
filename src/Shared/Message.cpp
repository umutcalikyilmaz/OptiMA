#include <Shared/Message.h>

namespace OptiMA
{
    Message::Message() { }

    Message::Message(string& prompt) : prompt(prompt) { }

    Message::Message(shared_ptr<Memory> parameters) : parameters(parameters) { }

    Message::Message(string& prompt, shared_ptr<Memory> parameters) : prompt(prompt), parameters(parameters) { }

    int Message::GetSenderId()
    {
        return senderId;
    }

    int Message::GetSenderType()
    {
        return senderType;
    }

    int Message::GetReceiverId()
    {
        return receiverId;
    }

    int Message::GetReceiverType()
    {
        return receiverType;
    }
}