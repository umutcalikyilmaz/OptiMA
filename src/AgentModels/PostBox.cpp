#include "OptiMA/AgentModels/PostBox.h"

namespace OptiMA
{
    void PostBox::sendMessage(PostmasterKey, std::shared_ptr<Message> msg)
    {
        messages_.push(msg);
    }

    std::queue<std::shared_ptr<Message>> PostBox::checkMessages(AgentKey)
    {        
        std::queue<std::shared_ptr<Message>> res;
        std::lock_guard<std::mutex> lock(postLock_);

        while(!messages_.empty())
        {
            res.push(messages_.front());
            messages_.pop();
        }

        return res;
    }
}