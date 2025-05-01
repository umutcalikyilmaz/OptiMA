#pragma once
#include <Shared/Message.h>

namespace OptiMA
{
    class Agent;
    class Postmaster;
    class ITransaction;
    class IAgentManager;

    class ITransactionFactory
    {
    public:
    
        virtual unique_ptr<ITransaction> GenerateCommunicateTransaction(Agent* owner, Postmaster* postmaster,
        shared_ptr<Message> msg, int receiverId) = 0;

        virtual unique_ptr<ITransaction> GenerateCommunicateTransaction(Agent* owner, Postmaster* postmaster,
        shared_ptr<Message> msg, int receiverIdOrType, bool toAll) = 0;

        virtual unique_ptr<ITransaction> GenerateCreateAgentTransaction(Agent* owner, IAgentManager* amanger,
        int targetType) = 0;

        virtual unique_ptr<ITransaction> GenerateCreateAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetType, shared_ptr<Memory> initialParameters) = 0;

        virtual unique_ptr<ITransaction> GenerateDestroyAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetId) = 0;

        virtual unique_ptr<ITransaction> GenerateGetAgentInfoTransaction(Agent* owner, IAgentManager* amanager,
        int targetId) = 0;

        virtual unique_ptr<ITransaction> GenerateGetAgentInfoTransaction(Agent* owner, IAgentManager* amanager,
        int targetIdOrType, bool fromAll) = 0;

        virtual unique_ptr<ITransaction> GenerateHaltProgramTransaction(Agent* owner, shared_ptr<Memory> outputParameters) = 0;

        virtual unique_ptr<ITransaction> GenerateStartAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetId, shared_ptr<Memory> initialParameters) = 0;

        virtual unique_ptr<ITransaction> GenerateStopAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetId) = 0;

        virtual unique_ptr<ITransaction> GenerateNullTransaction() = 0;

        virtual ~ITransactionFactory() = default;
    };
}