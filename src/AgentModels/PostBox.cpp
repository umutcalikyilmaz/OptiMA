#include "OptiMA/AgentModels/PostBox.h"

namespace OptiMA
{
    void PostBox::sendMessage(shared_ptr<Message> msg)
    {
        messages_.push(msg);
    }

    queue<shared_ptr<Message>> PostBox::checkMessages()
    {        
        queue<shared_ptr<Message>> res;
        lock_guard<mutex> lock(postLock_);

        while(!messages_.empty())
        {
            res.push(messages_.front());
            messages_.pop();
        }

        return res;
    }

    PostBox::~PostBox()
    {
        while(!messages_.empty())
        {
            messages_.pop();
        }
    }
}