#pragma once
#include "OptiMA/Shared/Message.h"

namespace OptiMA
{
    inline std::shared_ptr<Memory> generateMemory()
    {
        return std::make_shared<Memory>();
    }    

    inline std::shared_ptr<Message> generateMessage()
    {
        return std::make_shared<Message>();
    }

    inline std::shared_ptr<Message> generateMessage(std::string& prompt)
    {
        return std::make_shared<Message>(prompt);
    }

    inline std::shared_ptr<Message> generateMessage(std::shared_ptr<Memory> parameters)
    {
        return std::make_shared<Message>(parameters);
    }

    inline std::shared_ptr<Message> generateMessage(std::string& prompt, std::shared_ptr<Memory> parameters)
    {
        return std::make_shared<Message>(prompt, parameters);
    }
}