#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Inspector.h"
#include "OptiMA/TransactionModels/Transaction.h"

class ScanPart final : public Transaction
{
public:

    ScanPart(Agent* agent)
        : Transaction({agent}, 7, 0, {4}){ }

    std::shared_ptr<Memory> procedure() override
    {
        return executeInstruction(getSeizedAgents()[0], &Inspector::scanPart);
    }
};