#pragma once
#include "OptiMA/Shared/Message.h"

namespace OptiMA
{
    class PostBox
    {
    public:

        class PostmasterKey
        {
        private:

            PostmasterKey() {}

            friend class Postmaster;
        };

        class AgentKey
        {
        private:

            AgentKey() {}

            template <class A>
            friend class AgentTemplate;
        };

        void sendMessage(PostmasterKey, std::shared_ptr<Message> msg);

        std::queue<std::shared_ptr<Message>> checkMessages(AgentKey);

    private:

        std::queue<std::shared_ptr<Message>> messages_;
        std::mutex postLock_;
    };
}
