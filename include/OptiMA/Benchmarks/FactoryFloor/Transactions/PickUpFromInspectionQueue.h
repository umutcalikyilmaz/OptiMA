#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Inspector.h"
#include "OptiMA/TransactionModels/Transaction.h"

class PickUpFromInspectionQueue final : public Transaction
{
public:

    PickUpFromInspectionQueue()
        : Transaction(6, 0, {3}),
          ownerSet_(false) { }

    PickUpFromInspectionQueue(Agent* agent)
        : Transaction({agent}, 6, 0, {3}),
          ownerSet_(true) { }

    std::shared_ptr<Memory> procedure() override
    {
        std::shared_ptr<Memory> res;

        if(ownerSet_)
        {
            res = executeInstruction(getSeizedAgents()[0], &Inspector::pickUp);
        }
        else
        {
            res = executeInstruction(seizeAgent(2), &Inspector::pickUp);
            ownerSet_ = true;
        }

        return res;
    }

private:

    bool ownerSet_;
};