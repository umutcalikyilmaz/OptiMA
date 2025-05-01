#pragma once
#include <AgentModels/AgentTemplate.h>

namespace OptiMA
{
    class IAgentCoreFactory
    {
    public:
     
        virtual AgentCore* CreateAgentCore() = 0;
    };
}