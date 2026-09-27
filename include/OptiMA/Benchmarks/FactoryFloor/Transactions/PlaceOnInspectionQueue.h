#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Transporter.h"
#include "OptiMA/TransactionModels/Transaction.h"

class PlaceOnInspectionQueue final : public Transaction
{
public:

    PlaceOnInspectionQueue(Agent* agent)
        : Transaction({agent}, 5, 0, {3}) { }

    std::shared_ptr<Memory> procedure() override
    {
        return executeInstruction(getSeizedAgents()[0], &Transporter::place);
    }
};