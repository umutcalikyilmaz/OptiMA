#include "OptiMA/AgentModels/Agent.h"

namespace OptiMA
{
    Agent::Agent()
        : postBox_(std::make_unique<PostBox>()) { }

    void Agent::start(ManagerKey)
    {
        status_ = AgentStatus::ACTIVE;
        started_ = true;
    }

    void Agent::stop(ManagerKey)
    {
        started_ = false;
    }

    void Agent::setAgentId(ManagerKey, int agentId)
    {
        agentId_ = agentId;
    }

    void Agent::setCurrentTransaction(ManagerKey, long transactionId)
    {
        currentTransaction_ = transactionId;
    }

    AgentStatus Agent::getStatus(ManagerKey)
    {
        return status_;
    }

    PostBox* Agent::getPostBoxAddress(ManagerKey)
    {
        return postBox_.get();
    }

    void Agent::setAgentType(PoolKey, int agentType)
    {
        agentType_ = agentType;
    }

    void Agent::setCurrentTransaction(PoolKey)
    {
        currentTransaction_ = -1;
    }

    void Agent::setAgentManager(PoolKey, IAgentManager* amanger)
    {
        amanager_ = amanger;
    }

    void Agent::setPluginManager(PoolKey, PluginManager* pmanager)
    {
        pmanager_ = pmanager;
    }

    void Agent::setPostmaster(PoolKey, Postmaster* postmaster)
    {
        postmaster_ = postmaster;
    }

    void Agent::setSupervisors(PoolKey, const std::vector<int>& supervisors)
    {
        supervisors_ = supervisors;
    }

    void Agent::setSubordinates(PoolKey, const std::vector<int>& subordinates)
    {
        subordinates_ = subordinates;
    }

    void Agent::setCommunications(PoolKey, const std::vector<int>& contacts)
    {
        contacts_ = contacts;
    }

    void Agent::setTools(PoolKey, const std::vector<int>& tools)
    {
        allowedPlugins_ = tools;
    }

    void Agent::clearMemory(PoolKey)
    {
        clearMemory();
    }

    int Agent::getAgentId()
    {
        return agentId_;
    }
    /*
    int Agent::getAgentType()
    {
        return agentType_;
    }

    long Agent::getCurrentTransaction()
    {
        return currentTransaction_;
    }
    */
    
}