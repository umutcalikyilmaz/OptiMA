#include "OptiMA/AgentModels/Agent.h"

namespace OptiMA
{
    Agent::Agent() : postBox_(new PostBox()) { }

    void Agent::start()
    {
        status_ = AgentStatus::ACTIVE;
        started_ = true;
    }

    void Agent::stop()
    {
        started_ = false;
    }

    void Agent::setAgentId(int agentId)
    {
        agentId_ = agentId;
    }

    void Agent::setAgentType(int agentType)
    {
        agentType_ = agentType;
    }

    void Agent::setCurrentTransaction(long transactionId)
    {
        currentTransaction_ = transactionId;
    }

    void Agent::setAgentManager(IAgentManager* amanger)
    {
        amanager_ = amanger;
    }

    void Agent::setPluginManager(PluginManager* pmanager)
    {
        pmanager_ = pmanager;
    }

    void Agent::setPostmaster(Postmaster* postmaster)
    {
        postmaster_ = postmaster;
    }

    void Agent::setSupervisors(vector<int>& supervisors)
    {
        supervisors_ = supervisors;
    }

    void Agent::setSubordinates(vector<int>& subordinates)
    {
        subordinates_ = subordinates;
    }

    void Agent::setCommunications(vector<int>& contacts)
    {
        contacts_ = contacts;
    }

    void Agent::setTools(vector<int>& tools)
    {
        allowedPlugins_ = tools;
    }

    AgentStatus Agent::getStatus()
    {
        return status_;
    }
        
    int Agent::getAgentId()
    {
        return agentId_;
    }

    int Agent::getAgentType()
    {
        return agentType_;
    }

    long Agent::getCurrentTransaction()
    {
        return currentTransaction_;
    }

    PostBox* Agent::getPostBoxAddress()
    {
        return postBox_;
    }

    Agent::~Agent()
    {
        delete postBox_;
    }
}