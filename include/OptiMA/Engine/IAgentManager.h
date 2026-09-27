#pragma once
#include "OptiMA/Shared/Memory.h"

namespace OptiMA
{
    class IAgentManager
    {
    public:
        
        class AgentKey
        {
        private:

            AgentKey() {}
            
            template <class A>
            friend class AgentTemplate;
        };

        class TransactionKey
        {
        private:

            TransactionKey() {}

            friend class Transaction;
        };

        virtual void requestCreateAgent(AgentKey, long transactionId, int senderId, int senderType, int targetType) = 0;

        virtual void requestCreateAndStartAgent(AgentKey, long transactionId, int senderId, int senderType, int targetType) = 0;

        virtual void requestStartAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId) = 0;

        virtual void requestStopAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId) = 0;

        virtual void requestDestroyAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId) = 0;

        virtual std::shared_ptr<Memory> getAgentInfo(AgentKey, int senderId, int senderType, int targetId) = 0;

        virtual std::shared_ptr<Memory> getAgentInfos(AgentKey, int senderId, int senderType, int targetType) = 0;

        virtual void commit(TransactionKey, long transactionId) = 0;

        virtual void rollback(TransactionKey, long transactionId) = 0;

        virtual ~IAgentManager() = default;
    };
}
