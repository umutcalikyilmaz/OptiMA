#pragma once
#include <Engine/AgentManager.h>
#include <TransactionModels/ProceduralTransaction.h>

namespace OptiMA
{
    class StartAgent final : public ProceduralTransaction
    {
        IAgentManager* manager;
        shared_ptr<Memory> initialParameters;
        int targetId;
        
    public:

        StartAgent(Agent* owner, IAgentManager* manager, int targetId, shared_ptr<Memory> initialParameters);

        unique_ptr<TransactionResult> Procedure() override;
    };
}