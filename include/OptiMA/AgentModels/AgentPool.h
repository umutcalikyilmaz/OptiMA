#pragma once
#include "OptiMA/AgentModels/AgentFactory.h"
#include "OptiMA/Shared/Exceptions.h"

namespace OptiMA
{
    class AgentPool
    {
    public:

        Agent* getAgent();

        void returnAgent(Agent* agent);
        
        friend class AgentManager;

    private:

        IAgentManager* amanager_;
        PluginManager* pmanager_;
        Postmaster* postmaster_;
        IAgentFactory* factory_;
        vector<Agent*> agents_;
        queue<Agent*> agentQueue_;
        vector<int> supervisors_;
        vector<int> subordinates_;
        vector<int> phoneBook_;
        vector<int> toolBox_;
        int agentType_;
        int inUse_;

        AgentPool(IAgentFactory* factory, int agentType, vector<int>& supervisors, vector<int>& subordinates,
        vector<int>& phoneBook, vector<int>& toolBox, IAgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster);        

        ~AgentPool();
    };
}