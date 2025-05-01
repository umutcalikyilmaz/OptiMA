#include <Engine/TransactionFactory.h>

namespace OptiMA
{
    unique_ptr<ITransaction> TransactionFactory::GenerateCommunicateTransaction(Agent* owner, Postmaster* postmaster,
    shared_ptr<Message> msg, int receiverId)
    {
        return  make_unique<Communicate>(owner, postmaster, msg, receiverId);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateCommunicateTransaction(Agent* owner, Postmaster* postmaster,
    shared_ptr<Message> msg, int receiverIdOrType, bool toAll)
    {
        return make_unique<Communicate>(owner, postmaster, msg, receiverIdOrType, toAll);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateCreateAgentTransaction(Agent* owner, IAgentManager* amanager,
    int targetType)
    {
        return make_unique<CreateAgent>(owner, amanager, targetType);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateCreateAgentTransaction(Agent* owner, IAgentManager* amanager,
    int targetType, shared_ptr<Memory> initialParameters)
    {
        return make_unique<CreateAgent>(owner, amanager, targetType, initialParameters);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateDestroyAgentTransaction(Agent* owner, IAgentManager* amanager,
    int targetId)
    {
        return make_unique<DestroyAgent>(owner, amanager, targetId);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateGetAgentInfoTransaction(Agent* owner, IAgentManager* amanager,
    int targetId)
    {
        return make_unique<GetAgentInfo>(owner, amanager, targetId);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateGetAgentInfoTransaction(Agent* owner, IAgentManager* amanager,
    int targetIdOrType, bool fromAll)
    {
        return make_unique<GetAgentInfo>(owner, amanager, targetIdOrType, fromAll);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateHaltProgramTransaction(Agent* owner, shared_ptr<Memory> outputParameters)
    {
        return make_unique<HaltProgram>(owner, outputParameters);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateStartAgentTransaction(Agent* owner, IAgentManager* amanager,
    int targetId, shared_ptr<Memory> initialParameters)
    {
        return make_unique<StartAgent>(owner, amanager, targetId, initialParameters);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateStopAgentTransaction(Agent* owner, IAgentManager* amanager,
    int targetId)
    {
        return make_unique<StopAgent>(owner, amanager, targetId);
    }

    unique_ptr<ITransaction> TransactionFactory::GenerateNullTransaction()
    {
        return nullptr;
    }
}