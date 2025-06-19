#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Inspector.h"
#include "OptiMA/TransactionModels/Transaction.h"

class ScanPart : public Transaction
{
public:

    ScanPart(Agent* agent) : Transaction({agent}, 7, 0, {4}){ }

    shared_ptr<Memory> procedure() override
    {
        return executeInstruction(getSeizedAgents()[0], &Inspector::scanPart);
    }
};