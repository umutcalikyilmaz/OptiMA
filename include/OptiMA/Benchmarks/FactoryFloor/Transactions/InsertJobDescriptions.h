#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/FloorManager.h"
#include "OptiMA/TransactionModels/Transaction.h"

class InsertJobDescriptions : public Transaction
{
public:

    InsertJobDescriptions() : Transaction(9, 0, {0}) { }

    shared_ptr<Memory> procedure() override
    {
        Agent* agent = seizeAgent(3);
        executeInstruction(agent, &FloorManager::insertJobs);
        releaseAgent(agent);
        return nullptr;
    }
};