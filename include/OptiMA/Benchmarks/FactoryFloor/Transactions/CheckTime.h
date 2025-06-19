#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/FloorManager.h"
#include "OptiMA/TransactionModels/Transaction.h"

class CheckTime : public Transaction
{
private:

    bool ownerSet_;

public:

    CheckTime() : Transaction(12, 0, {}), ownerSet_(false) { }

    CheckTime(Agent* agent) : Transaction({agent}, 12, 0, {}), ownerSet_(true) { }

    shared_ptr<Memory> procedure() override
    {
        if(ownerSet_)
        {
            return executeInstruction(getSeizedAgents()[0], &FloorManager::checkTime);
        }
        else
        {
            return executeInstruction(seizeAgent(3), &FloorManager::checkTime);
        }
    }
};