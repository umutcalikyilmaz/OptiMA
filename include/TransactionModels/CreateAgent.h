#pragma once
#include <Engine/IAgentManager.h>
#include <TransactionModels/ProceduralTransaction.h>

namespace OptiMA
{
    class CreateAgent final : public ProceduralTransaction
    {
        IAgentManager* manager;
        shared_ptr<Memory>  initialParameters;
        int targetType;
        bool begin;
        bool initialized;

    public:
        CreateAgent(Agent* owner, IAgentManager* manager, int targetType);

        CreateAgent(Agent* owner, IAgentManager* manager, int targetType, shared_ptr<Memory> initialParameters);

        unique_ptr<TransactionResult> Procedure() override;
    };
}