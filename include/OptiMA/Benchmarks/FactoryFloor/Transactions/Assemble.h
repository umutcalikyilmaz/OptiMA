#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/AssemblyWorker.h"
#include "OptiMA/TransactionModels/Transaction.h"

class Assemble final : public Transaction
{
public:

    Assemble(Agent* agent, int subtype, std::vector<std::pair<OperationType, int>> description,
        std::set<int> requestedPlugins)
        : Transaction({agent}, 1, subtype, requestedPlugins), description_(description){ }

    std::shared_ptr<Memory> procedure() override
    {
        std::shared_ptr<Memory> res;
        res = executeInstruction(getSeizedAgents()[0], &AssemblyWorker::operate, description_);
        return res;
    }

private:

    std::vector<std::pair<OperationType, int>> description_;
};
