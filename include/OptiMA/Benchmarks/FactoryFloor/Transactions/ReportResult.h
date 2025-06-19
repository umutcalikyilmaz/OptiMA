#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Inspector.h"
#include "OptiMA/TransactionModels/Transaction.h"

class ReportResult : public Transaction
{
public:

    ReportResult(Agent* agent) : Transaction({agent}, 8, 0, {6}){ }

    shared_ptr<Memory> procedure() override
    {
        shared_ptr<Memory> res = generateMemory();
        Agent* agent = getSeizedAgents()[0];
        shared_ptr<Memory> operationResult = executeInstruction(agent, &Inspector::reportResult);
        bool stopped = get<0>(operationResult->getTuple<bool>(0));

        operationResult = executeInstruction(agent, &Inspector::place);

        if(get<0>(operationResult->getTuple<bool>(0)))
        {
            haltProgram(nullptr);
        }

        if(stopped)
        {
            Agent* agent = getSeizedAgents()[0];
            executeInstruction(agent, &Inspector::selfStop);
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