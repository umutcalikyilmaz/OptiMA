#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/FloorManager.h"
#include "OptiMA/TransactionModels/Transaction.h"

class ManageConveyorBelt final : public Transaction
{
public:

    ManageConveyorBelt(Agent* agent)
        : Transaction({agent}, 10, 0, {1}) { }
    
    std::shared_ptr<Memory> procedure() override
    {
        executeInstruction(getSeizedAgents()[0], &FloorManager::manageConveyorBelt);
        return nullptr;
    }
};