#pragma once
#include <Engine/AgentManager.h>
#include <TransactionModels/ProceduralTransaction.h>

namespace OptiMA
{
    class DestroyAgent final : public ProceduralTransaction
    {
        IAgentManager* manager;
        int targetId;

    public:

        DestroyAgent(Agent* owner, IAgentManager* manager, int targetId);

        unique_ptr<TransactionResult> Procedure() override;
    };
}