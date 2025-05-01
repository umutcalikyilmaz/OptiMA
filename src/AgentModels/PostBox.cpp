#include <AgentModels/PostBox.h>

namespace OptiMA
{
    queue<Message*> messages;
    vector<Message*> trash;
    atomic_bool lock;

    void PostBox::SendMessage(shared_ptr<Message> msg)
    {
        messages.push(msg);
    }

    queue<shared_ptr<Message>> PostBox::CheckMessages()
    {        
        queue<shared_ptr<Message>> res;
        lock_guard<mutex> lock(postLock);

        while(!messages.empty())
        {
            res.push(messages.front());
            messages.pop();
        }

        return res;
    }

    PostBox::~PostBox()
    {
        while(!messages.empty())
        {
            messages.pop();
        }
    }
}