#include "OptiMA/AgentModels/AgentPool.h"

namespace OptiMA
{
    AgentPool::AgentPool(IAgentFactory* factory, int agentType, vector<int>& supervisors, vector<int>& subordinates,
    vector<int>& phoneBook, vector<int>& toolBox, IAgentManager* amanager, PluginManager* pmanager, Postmaster* postmaster)
    : factory_(factory), agentType_(agentType), inUse_(0), supervisors_(supervisors), subordinates_(subordinates),
    phoneBook_(phoneBook), toolBox_(toolBox), amanager_(amanager), pmanager_(pmanager), postmaster_(postmaster) { }

    Agent* AgentPool::getAgent()
    {
        Agent* res;

        if(agentQueue_.empty())
        {
            res = factory_->createAgent();
            res->setSupervisors(supervisors_);
            res->setSubordinates(subordinates_);
            res->setCommunications(phoneBook_);
            res->setTools(toolBox_);
            res->setAgentManager(amanager_);
            res->setPluginManager(pmanager_);
            res->setPostmaster(postmaster_);
            res->setCurrentTransaction(-1);
            agents_.push_back(res);
        }
        else
        {
            res = agentQueue_.front();
            agentQueue_.pop();
        }
        
        res->setAgentType(agentType_);
        inUse_++;
        return res;
    }

    void AgentPool::returnAgent(Agent* agent)
    {
        agent->clearMemory();
        agentQueue_.push(agent);
    }

    AgentPool::~AgentPool()
    {
        for(Agent* agent : agents_)
        {
            delete agent;
        }
    }
}