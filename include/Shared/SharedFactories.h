#pragma once
#include <Shared/Message.h>

namespace OptiMA
{
    /*
    inline shared_ptr<Record> GenerateRecord()
    {
        return make_shared<Record>();
    }
    */

    inline shared_ptr<Memory> GenerateMemory()
    {
        return make_shared<Memory>();
    }    

    inline shared_ptr<Message> GenerateMessage()
    {
        return make_shared<Message>();
    }

    inline shared_ptr<Message> GenerateMessage(string& prompt)
    {
        return make_shared<Message>(prompt);
    }

    inline shared_ptr<Message> GenerateMessage(shared_ptr<Memory> parameters)
    {
        return make_shared<Message>(parameters);
    }

    inline shared_ptr<Message> GenerateMessage(string& prompt, shared_ptr<Memory> parameters)
    {
        return make_shared<Message>(prompt, parameters);
    }
}