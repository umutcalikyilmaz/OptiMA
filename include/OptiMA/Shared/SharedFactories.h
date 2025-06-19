#pragma once
#include "OptiMA/Shared/Message.h"

namespace OptiMA
{
    inline shared_ptr<Memory> generateMemory()
    {
        return make_shared<Memory>();
    }    

    inline shared_ptr<Message> generateMessage()
    {
        return make_shared<Message>();
    }

    inline shared_ptr<Message> generateMessage(string& prompt)
    {
        return make_shared<Message>(prompt);
    }

    inline shared_ptr<Message> generateMessage(shared_ptr<Memory> parameters)
    {
        return make_shared<Message>(parameters);
    }

    inline shared_ptr<Message> generateMessage(string& prompt, shared_ptr<Memory> parameters)
    {
        return make_shared<Message>(prompt, parameters);
    }
}