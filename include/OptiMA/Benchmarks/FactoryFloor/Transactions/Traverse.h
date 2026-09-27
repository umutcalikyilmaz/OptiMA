#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Transporter.h"
#include "OptiMA/TransactionModels/Transaction.h"

class Traverse final : public Transaction
{
public:

    Traverse(Agent* agent, bool returnJourney) : Transaction({agent}, 4, 0, {}), returnJourney_(returnJourney) { }

    std::shared_ptr<Memory> procedure() override
    {
        std::shared_ptr res = std::make_shared<Memory>();
        std::shared_ptr<Memory> operationResult = executeInstruction(getSeizedAgents()[0], &Transporter::traverse);
        
        if(returnJourney_ && get<0>(operationResult->getTuple<bool>(0)))
        {
            Agent* agent = getSeizedAgents()[0];
            executeInstruction(agent, &Transporter::selfStop);
            releaseAgent(agent);
            res->addTuple(returnJourney_, false);
        }
        else
        {
            res->addTuple(returnJourney_, true);
        }

        return res;
    }

private:

    bool returnJourney_;
};