#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Transporter.h"
#include "OptiMA/TransactionModels/Transaction.h"

class PickUpFromConveyorBelt final : public Transaction
{
public:
    PickUpFromConveyorBelt() : Transaction(3, 0, {1}), ownerSet_(false) { }

    PickUpFromConveyorBelt(Agent* agent) : Transaction({agent}, 3, 0, {1}), ownerSet_(true) { }

    std::shared_ptr<Memory> procedure() override
    {
        std::shared_ptr<Memory> res;

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

private:

    bool ownerSet_;
};
