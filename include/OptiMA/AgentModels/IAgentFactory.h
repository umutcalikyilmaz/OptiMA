#pragma once
#include "OptiMA/AgentModels/AgentTemplate.h"

namespace OptiMA
{
    class IAgentFactory
    {
    public:
     
        virtual Agent* createAgent() = 0;
    };
}