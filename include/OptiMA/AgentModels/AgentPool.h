#pragma once
#include "OptiMA/AgentModels/AgentFactory.h"
#include "OptiMA/Shared/Exceptions.h"

namespace OptiMA
{
    class AgentPool
    {
    public:

        class AgentManagerKey
        {
        private:

            AgentManagerKey() {}

            friend class AgentManager;
        };

        AgentPool(AgentManagerKey, IAgentFactory* factory, int agentType,
            const std::vector<int>& supervisors, const std::vector<int>& subordinates,
            const std::vector<int>& phoneBook, const std::vector<int>& toolBox, IAgentManager* amanager,
            PluginManager* pmanager, Postmaster* postmaster);

        std::unique_ptr<Agent> getAgent(AgentManagerKey);

        void returnAgent(AgentManagerKey, std::unique_ptr<Agent> agent);

    private:

        IAgentManager* amanager_;
        PluginManager* pmanager_;
        Postmaster* postmaster_;
        IAgentFactory* factory_;
        std::queue<std::unique_ptr<Agent>> agentQueue_;
        const std::vector<int> supervisors_;
        const std::vector<int> subordinates_;
        const std::vector<int> phoneBook_;
        const std::vector<int> toolBox_;
        int agentType_;
        int inUse_;

        
    };
}