#pragma once
#include <AgentModels/IAgentCoreFactory.h>

namespace OptiMA
{
    template <typename T, typename = std::enable_if_t< std::is_base_of<AgentTemplate<std::decay_t<T> >, std::decay_t<T>>::value>>
    class AgentCoreFactory : public IAgentCoreFactory
    {        
        using AgentType = std::decay_t<T>;
    
    public:

        AgentCore* CreateAgentCore() override
        {
            return (AgentCore*)std::make_unique<AgentType>().release();
        }
    };
}