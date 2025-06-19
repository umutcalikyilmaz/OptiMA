#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/AssemblyWorker.h"
#include "OptiMA/TransactionModels/Transaction.h"

class PlaceOnConveyorBelt : public Transaction
{
public:

    PlaceOnConveyorBelt(Agent* agent) : Transaction({agent}, 2, 0, {1}) { }

    shared_ptr<Memory> procedure() override
    {
        shared_ptr<Memory> res = make_shared<Memory>();
        shared_ptr<Memory> operationResult = executeInstruction(getSeizedAgents()[0], &AssemblyWorker::placeOnConveyorBelt);        

        if(get<0>(operationResult->getTuple<bool>(0)))
        {
            Agent* agent = getSeizedAgents()[0];
            executeInstruction(agent, &AssemblyWorker::selfStop);
            releaseAgent(agent);
            res->addTuple(false);
        }
        else
        {
            res->addTuple(true);
        }

        return res;
    }
};