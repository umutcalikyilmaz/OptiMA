#include <AgentModels/CorePool.h>

namespace OptiMA
{
    CorePool::CorePool(IAgentCoreFactory* factory, int agentType, vector<int>& supervisors, vector<int>& subordinates,
    vector<int>& phoneBook, vector<int>& toolBox, IAgentManager* amanager, ITransactionFactory* tfactory, PluginManager* pmanager,
    Postmaster* postmaster) : factory(factory), agentType(agentType), inUse(0), supervisors(supervisors), subordinates(subordinates),
    phoneBook(phoneBook), toolBox(toolBox), amanager(amanager), tfactory(tfactory), pmanager(pmanager), postmaster(postmaster) { }

    AgentCore* CorePool::GetCore()
    {
        AgentCore* res;

        if(coreQueue.empty())
        {
            res = factory->CreateAgentCore();
            res->SetSupervisors(supervisors);
            res->SetSubordinates(subordinates);
            res->SetCommunications(phoneBook);
            res->SetTools(toolBox);
            res->SetAgentManager(amanager);
            res->SetTransactionFactory(tfactory);
            res->SetPluginManager(pmanager);
            res->SetPostmaster(postmaster);
            cores.push_back(res);
        }
        else
        {
            res = coreQueue.front();
            coreQueue.pop();
        }
        
        res->SetAgentType(agentType);
        inUse++;
        return res;
    }

    void CorePool::ReturnCore(AgentCore* core)
    {
        core->ClearMemory();
        coreQueue.push(core);
    }

    CorePool::~CorePool()
    {
        for(AgentCore* core : cores)
        {
            delete core;
        }
    }
}