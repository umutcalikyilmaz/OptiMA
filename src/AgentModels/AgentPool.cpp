#include "OptiMA/AgentModels/AgentPool.h"

namespace OptiMA
{
    AgentPool::AgentPool(AgentManagerKey, IAgentFactory* factory, int agentType,
        const std::vector<int>& supervisors, const std::vector<int>& subordinates,
        const std::vector<int>& phoneBook, const std::vector<int>& toolBox, IAgentManager* amanager,
        PluginManager* pmanager, Postmaster* postmaster)
        : factory_(factory),
          agentType_(agentType),
          inUse_(0),
          supervisors_(supervisors),
          subordinates_(subordinates),
          phoneBook_(phoneBook),
          toolBox_(toolBox),
          amanager_(amanager),
          pmanager_(pmanager),
          postmaster_(postmaster) { }

    std::unique_ptr<Agent> AgentPool::getAgent(AgentManagerKey)
    {
        std::unique_ptr<Agent> res;

        if(agentQueue_.empty())
        {
            res = factory_->createAgent();
            res->setSupervisors(typename Agent::PoolKey {}, supervisors_);
            res->setSubordinates(typename Agent::PoolKey {}, subordinates_);
            res->setCommunications(typename Agent::PoolKey {}, phoneBook_);
            res->setTools(typename Agent::PoolKey {}, toolBox_);
            res->setAgentManager(typename Agent::PoolKey {}, amanager_);
            res->setPluginManager(typename Agent::PoolKey {}, pmanager_);
            res->setPostmaster(typename Agent::PoolKey {}, postmaster_);
            res->setCurrentTransaction(typename Agent::PoolKey {});
        }
        else
        {
            res = std::move(agentQueue_.front());
            agentQueue_.pop();
        }
        
        res->setAgentType(typename Agent::PoolKey {}, agentType_);
        inUse_++;
        return std::move(res);
    }

    void AgentPool::returnAgent(AgentManagerKey, std::unique_ptr<Agent> agent)
    {
        agent->clearMemory(typename Agent::PoolKey {});
        agentQueue_.push(std::move(agent));
    }
}