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

        class AgentManagerKey
        {
        private:

            AgentManagerKey() {}

            friend class AgentManager;
        };

        class AgentKey
        {
        private:
            
            AgentKey() {}

            template<class A>
            friend class AgentTemplate;
        };

        class TransactionKey
        {
            TransactionKey() {}

            friend class Transaction;
        };

        Postmaster(AgentManagerKey, std::map<int,std::vector<int>>& agentIds, std::map<int,
            std::vector<int>>& communicators,  int startingTime);

        void addAgent(AgentManagerKey, int agentId, int agentType, PostBox* postbox);

        //void removeAgent(int agentId, int agentType);

        void sendToId(AgentKey, long transactionId, int senderId, int senderType, int receiverId,
            std::shared_ptr<Message> msg);

        void sendToType(AgentKey, long transactionId, int senderId, int senderType, int receiverType,
            std::shared_ptr<Message> msg);

        void commit(TransactionKey, long transactionId);

        void rollback(TransactionKey, long transactionId);

        ~Postmaster();

    private:

        std::map<int,std::pair<PostBox*,int>> postBoxes_;
        std::map<int,std::vector<int>> communicators_;
        std::map<int,std::vector<int>> agentIds_;
        std::map<long,std::vector<std::pair<std::shared_ptr<Message>, PostBox*>>> transactionLog_;
        long startingTime_;
        std::mutex postLock_;
        std::mutex logLock_;        

        bool checkSender(int receiverType, int senderType);

        void enterLog(long transactionId, int senderId, int senderType, int receiverId, int receiverType,
            std::shared_ptr<Message> msg, PostBox* box);

        void send(std::shared_ptr<Message> msg, PostBox* postbox);
    };
}