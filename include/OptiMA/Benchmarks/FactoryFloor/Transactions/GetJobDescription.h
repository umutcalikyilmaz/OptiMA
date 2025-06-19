#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/AssemblyWorker.h"
#include "OptiMA/TransactionModels/Transaction.h"

class GetJobDescription : public Transaction
{
private:

    bool ownerSet_;

public:

    GetJobDescription() : Transaction(0, 0, {0}), ownerSet_(false) { }

    GetJobDescription(Agent* agent) : Transaction({agent}, 0, 0, {0}), ownerSet_(true) { }

    shared_ptr<Memory> procedure() override
    {
        shared_ptr<Memory> res;

        if(ownerSet_)
        {
            res = executeInstruction(getSeizedAgents()[0], &AssemblyWorker::retrieveJob);
        }
        else
        {
            res = executeInstruction(seizeAgent(0), &AssemblyWorker::retrieveJob);
            ownerSet_ = true;
        }

        return res;
    }
};
