#include <Engine/Postmaster.h>

namespace OptiMA
{
    bool Postmaster::CheckSender(int receiverType, int senderType)
    {
        bool found = false;

        for(int com : communicators[receiverType])
        {
            if(com == senderType)
            {
                found = true;
                break;
            }
        }

        return found;
    }

    void Postmaster::Send(int senderId, int senderType, int receiverId, int receiverType, PostBox* postbox,
    shared_ptr<Message> msg)
    {
        msg->senderId = senderId;
        msg->senderType = senderType;
        msg->receiverId = receiverId;
        msg->receiverType = receiverType;
        msg->timeStamp = chrono::steady_clock::now().time_since_epoch().count() - startingTime;
        postbox->SendMessage(msg);
    }

    Postmaster::Postmaster(map<int,vector<int>>& agentIds, map<int,
    vector<int>>& communicators, int startingTime) : agentIds(agentIds), communicators(communicators),
    startingTime(startingTime) { }

    void Postmaster::AddAgent(int agentId, int agentType, PostBox* postbox)
    {
        lock_guard<mutex> lock(postLock);
        postBoxes[agentId] = make_pair(postbox, agentType);
        agentIds[agentType].push_back(agentId);
    }

    void Postmaster::RemoveAgent(int agentId, int agentType)
    {
        lock_guard<mutex> lock(postLock);
        postBoxes.erase(agentId);
        agentIds[agentType].erase(remove(agentIds[agentType].begin(), agentIds[agentType].end(), agentId), agentIds[agentType].end());        
    }

    void Postmaster::SendToId(int senderId, int senderType, int receiverId, shared_ptr<Message> msg)
    {
        lock_guard<mutex> lock(postLock);
        auto box = postBoxes[receiverId];
        int receiverType = box.second;

        if(!CheckSender(receiverType, senderType))
        {
            throw UnautorizedAccessException((char*)"The sender is not autorized to communicate with this type of agent");
        }

        Send(senderId, senderType, receiverId, receiverType, box.first, msg);
    }

    void Postmaster::SendToType(int senderId, int senderType, int receiverType, shared_ptr<Message> msg)
    {
        lock_guard<mutex> lock(postLock);

        if(!CheckSender(receiverType, senderType))
        {
            throw UnautorizedAccessException((char*)"The sender is not autorized to communicate with this type of agent");
        }

        for(int receiverId : agentIds[receiverType])
        {
            Send(senderId, senderType, receiverId, receiverType, postBoxes[receiverId].first, msg);
        }
    }

    Postmaster::~Postmaster(){ }
}