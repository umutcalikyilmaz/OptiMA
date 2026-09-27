#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/FloorManager.h"
#include "OptiMA/TransactionModels/Transaction.h"

class InsertJobDescriptions final : public Transaction
{
public:

    InsertJobDescriptions()
        : Transaction(9, 0, {0}) { }

    std::shared_ptr<Memory> procedure() override
    {
        Agent* agent = seizeAgent(3);
        executeInstruction(agent, &FloorManager::insertJobs);
        releaseAgent(agent);
        return nullptr;
    }
};