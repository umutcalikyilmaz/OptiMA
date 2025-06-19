#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Inspector.h"
#include "OptiMA/TransactionModels/Transaction.h"

class PickUpFromInspectionQueue : public Transaction
{
private:

    bool ownerSet_;

public:

    PickUpFromInspectionQueue() : Transaction(6, 0, {3}), ownerSet_(false) { }

    PickUpFromInspectionQueue(Agent* agent) : Transaction({agent}, 6, 0, {3}), ownerSet_(true) { }

    shared_ptr<Memory> procedure() override
    {
        shared_ptr<Memory> res;

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
};