#pragma once
#include "OptiMA/Shared/Message.h"

namespace OptiMA
{
    class PostBox
    {
    public:

        void sendMessage(shared_ptr<Message> msg);

        queue<shared_ptr<Message>> checkMessages();

        ~PostBox();

    private:

        queue<shared_ptr<Message>> messages_;
        mutex postLock_;
    };
}
