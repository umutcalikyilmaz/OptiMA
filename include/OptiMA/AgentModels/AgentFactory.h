#pragma once
#include "OptiMA/AgentModels/IAgentFactory.h"

namespace OptiMA
{
    template <
        typename T,
        typename = std::enable_if_t< std::is_base_of<AgentTemplate<std::decay_t<T> >, std::decay_t<T>>::value>>
    class AgentFactory : public IAgentFactory
    {        
        using AgentType = std::decay_t<T>;
    
    public:

        std::unique_ptr<Agent> createAgent() override
        {
            return std::make_unique<AgentType>();
        }
    };
}