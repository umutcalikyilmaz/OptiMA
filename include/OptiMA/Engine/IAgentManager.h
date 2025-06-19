#pragma once
#include "OptiMA/Shared/Memory.h"

namespace OptiMA
{
    class IAgentManager
    {
    public:

        //virtual Agent* assignAgentByType(long transactionId, int agentType) = 0;

        //virtual Agent* assignAgentById(long transactionId, int agentId) = 0;

        //virtual void releaseAgent(int agentId) = 0;
        
        virtual void requestCreateAgent(long transactionId, int senderId, int senderType, int targetType) = 0;

        virtual void requestCreateAndStartAgent(long transactionId, int senderId, int senderType, int targetType) = 0;

        virtual void requestStartAgent(long transactionId, int senderId, int senderType, int targetId) = 0;

        virtual void requestStopAgent(long transactionId, int senderId, int senderType, int targetId) = 0;

        virtual void requestDestroyAgent(long transactionId, int senderId, int senderType, int targetId) = 0;

        virtual shared_ptr<Memory> getAgentInfo(int senderId, int senderType, int targetId) = 0;

        virtual shared_ptr<Memory> getAgentInfos(int senderId, int senderType, int targetType) = 0;

        virtual void commit(long transactionId) = 0;

        virtual void rollback(long transactionId) = 0;

        virtual ~IAgentManager() = default;
    };
}
