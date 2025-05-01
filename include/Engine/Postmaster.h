#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <AgentModels/PostBox.h>


namespace OptiMA
{
    class Postmaster
    {        
        map<int,pair<PostBox*,int>> postBoxes;
        map<int,vector<int>> communicators;
        map<int,vector<int>> agentIds;
        long startingTime;
        mutex postLock;

        bool CheckSender(int receiverType, int senderType);

        void Send(int senderId, int senderType, int receiverId, int receiverType, PostBox* postbox, shared_ptr<Message> msg);

    public:

        Postmaster(map<int,vector<int>>& agentIds, map<int,
        vector<int>>& communicators, int startingTime);

        void AddAgent(int agentId, int agentType, PostBox* postbox);

        void RemoveAgent(int agentId, int agentType);

        void SendToId(int senderId, int senderType, int receiverId, shared_ptr<Message>  msg);

        void SendToType(int senderId, int senderType, int receiverType, shared_ptr<Message> msg);

        ~Postmaster();
    };
}