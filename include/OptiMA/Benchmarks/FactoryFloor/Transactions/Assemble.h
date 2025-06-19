#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/AssemblyWorker.h"
#include "OptiMA/TransactionModels/Transaction.h"

class Assemble : public Transaction
{
private:

    vector<pair<OperationType, int>> description_;

public:

    Assemble(Agent* agent, int subtype, vector<pair<OperationType, int>> description, set<int> requestedPlugins) : Transaction({agent}, 1,
    subtype, requestedPlugins), description_(description){ }

    shared_ptr<Memory> procedure() override
    {
        shared_ptr<Memory> res;
        res = executeInstruction(getSeizedAgents()[0], &AssemblyWorker::operate, description_);
        return res;
    }
};
