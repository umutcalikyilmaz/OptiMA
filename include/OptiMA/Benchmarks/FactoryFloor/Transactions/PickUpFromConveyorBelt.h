#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Transporter.h"
#include "OptiMA/TransactionModels/Transaction.h"

class PickUpFromConveyorBelt : public Transaction
{
private:

    bool ownerSet_;

public:
    PickUpFromConveyorBelt() : Transaction(3, 0, {1}), ownerSet_(false) { }

    PickUpFromConveyorBelt(Agent* agent) : Transaction({agent}, 3, 0, {1}), ownerSet_(true) { }

    shared_ptr<Memory> procedure() override
    {
        shared_ptr<Memory> res;

        if(ownerSet_)
        {
            res = executeInstruction(getSeizedAgents()[0], &Transporter::pickUp);
        }
        else
        {
            res = executeInstruction(seizeAgent(1), &Transporter::pickUp);
            ownerSet_ = true;
        }

        return res;
    }
};
