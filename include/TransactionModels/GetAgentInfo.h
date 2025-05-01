#pragma once
#include <Engine/AgentManager.h>
#include <TransactionModels/ProceduralTransaction.h>

namespace OptiMA
{
    class GetAgentInfo final : public ProceduralTransaction
    {
        IAgentManager* manager;
        int targetIdOrType;
        bool forAll;

    public:

        GetAgentInfo(Agent* owner, IAgentManager* manager, int targetId);

        GetAgentInfo(Agent* owner, IAgentManager* manager, int targetIdOrType, bool fromAll);

        unique_ptr<TransactionResult> Procedure() override;
    };
}