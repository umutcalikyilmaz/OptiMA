#pragma once
#include <Shared/Message.h>

namespace OptiMA
{
    class PostBox
    {
        queue<shared_ptr<Message>> messages;
        mutex postLock;

    public:

        void SendMessage(shared_ptr<Message> msg);

        queue<shared_ptr<Message>> CheckMessages();

        ~PostBox();
    };
}
