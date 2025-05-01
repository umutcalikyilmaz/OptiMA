#pragma once
#include <Shared/Memory.h>

namespace OptiMA
{
    class IAgentManager
    {
    public:

        virtual void CreateAgent(int senderId, int senderType, int targetType) = 0;

        virtual void CreateAgent(int senderId, int senderType, int targetType, shared_ptr<Memory> initialParameters) = 0;

        virtual void StartAgent(int senderId, int senderType, int targetId, shared_ptr<Memory> initialParameters) = 0;

        virtual void StopAgent(int senderId, int senderType, int targetId) = 0;

        virtual void DestroyAgent(int senderId, int senderType, int targetId) = 0;

        virtual shared_ptr<Memory> GetAgentInfo(int senderId, int senderType, int targetId) = 0;

        virtual shared_ptr<Memory> GetAgentInfos(int senderId, int senderType, int targetType) = 0;

        virtual ~IAgentManager() = default;
    };
}
