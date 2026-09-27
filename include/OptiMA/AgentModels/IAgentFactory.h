#pragma once
#include "OptiMA/AgentModels/AgentTemplate.h"

namespace OptiMA
{
    class IAgentFactory
    {
    public:
     
        virtual std::unique_ptr<Agent> createAgent() = 0;
    };
}