#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/FloorManager.h"
#include "OptiMA/TransactionModels/Transaction.h"

class ManageConveyorBelt : public Transaction
{
public:
    ManageConveyorBelt(Agent* agent) : Transaction({agent}, 10, 0, {1}) { }
    
    shared_ptr<Memory> procedure() override
    {
        executeInstruction(getSeizedAgents()[0], &FloorManager::manageConveyorBelt);
        return nullptr;
    }
};