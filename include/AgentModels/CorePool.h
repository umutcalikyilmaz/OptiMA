#pragma once
#include <AgentModels/AgentCoreFactory.h>
#include <Shared/Exceptions.h>

namespace OptiMA
{
    class CorePool final
    {
        IAgentManager* amanager;
        ITransactionFactory* tfactory;
        PluginManager* pmanager;
        Postmaster* postmaster;
        IAgentCoreFactory* factory;
        vector<AgentCore*> cores;
        queue<AgentCore*> coreQueue;
        vector<int> supervisors;
        vector<int> subordinates;
        vector<int> phoneBook;
        vector<int> toolBox;
        int agentType;
        int inUse;

        CorePool(IAgentCoreFactory* factory, int agentType, vector<int>& supervisors, vector<int>& subordinates,
        vector<int>& phoneBook, vector<int>& toolBox, IAgentManager* amanager, ITransactionFactory* tfactory,
        PluginManager* pmanager, Postmaster* postmaster);

        AgentCore* GetCore();

        ~CorePool();

    public:

        void ReturnCore(AgentCore* core);
        
        friend class AgentManager;
    };
}