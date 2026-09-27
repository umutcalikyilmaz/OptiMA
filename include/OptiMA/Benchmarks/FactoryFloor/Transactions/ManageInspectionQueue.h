#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/FloorManager.h"
#include "OptiMA/TransactionModels/Transaction.h"

class ManageInspectionQueue final : public Transaction
{
public:

    ManageInspectionQueue(Agent* agent)
        : Transaction({agent}, 11, 0, {3}) { }

    std::shared_ptr<Memory> procedure() override
    {
        executeInstruction(getSeizedAgents()[0], &FloorManager::manageInspectionQueue);
        return nullptr;
    }
};