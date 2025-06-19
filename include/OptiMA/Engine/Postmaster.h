#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include "OptiMA/AgentModels/PostBox.h"


namespace OptiMA
{
    class Postmaster
    {        
    public:

        Postmaster(map<int,vector<int>>& agentIds, map<int,
        vector<int>>& communicators, int startingTime);

        void addAgent(int agentId, int agentType, PostBox* postbox);

        void removeAgent(int agentId, int agentType);

        void sendToId(long transactionId, int senderId, int senderType, int receiverId, shared_ptr<Message>  msg);

        void sendToType(long transactionId, int senderId, int senderType, int receiverType, shared_ptr<Message> msg);

        void commit(long transactionId);

        void rollback(long transactionId);

        ~Postmaster();

    private:

        map<int,pair<PostBox*,int>> postBoxes_;
        map<int,vector<int>> communicators_;
        map<int,vector<int>> agentIds_;
        map<long,vector<pair<shared_ptr<Message>, PostBox*>>> transactionLog_;
        long startingTime_;
        mutex postLock_;
        mutex logLock_;        

        bool checkSender(int receiverType, int senderType);

        void enterLog(long transactionId, int senderId, int senderType, int receiverId, int receiverType, shared_ptr<Message> msg, PostBox* box);

        void send(shared_ptr<Message> msg, PostBox* postbox);
    };
}