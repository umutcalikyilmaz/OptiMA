#pragma once
#include <Engine/ITransactionFactory.h>
#include <Engine/Postmaster.h>
#include <TransactionModels/Communicate.h>
#include <TransactionModels/CreateAgent.h>
#include <TransactionModels/DestroyAgent.h>
#include <TransactionModels/GetAgentInfo.h>
#include <TransactionModels/HaltProgram.h>
#include <TransactionModels/StartAgent.h>
#include <TransactionModels/StopAgent.h>

namespace OptiMA
{
    class TransactionFactory : public ITransactionFactory
    {
    public:
        unique_ptr<ITransaction> GenerateCommunicateTransaction(Agent* owner, Postmaster* postmaster,
        shared_ptr<Message> msg, int receiverId) override;

        unique_ptr<ITransaction> GenerateCommunicateTransaction(Agent* owner, Postmaster* postmaster,
        shared_ptr<Message> msg, int receiverIdOrType, bool toAll) override;

        unique_ptr<ITransaction> GenerateCreateAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetType) override;

        unique_ptr<ITransaction> GenerateCreateAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetType, shared_ptr<Memory> initialParameters) override;

        unique_ptr<ITransaction> GenerateDestroyAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetId) override;

        unique_ptr<ITransaction> GenerateGetAgentInfoTransaction(Agent* owner, IAgentManager* amanager,
        int targetId) override;

        unique_ptr<ITransaction> GenerateGetAgentInfoTransaction(Agent* owner, IAgentManager* amanager,
        int targetIdOrType, bool fromAll) override;

        unique_ptr<ITransaction> GenerateHaltProgramTransaction(Agent* owner, shared_ptr<Memory> outputParameters) override;

        unique_ptr<ITransaction> GenerateStartAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetId, shared_ptr<Memory> initialParameters) override;

        unique_ptr<ITransaction> GenerateStopAgentTransaction(Agent* owner, IAgentManager* amanager,
        int targetId) override;

        unique_ptr<ITransaction> GenerateNullTransaction() override;
    };
}