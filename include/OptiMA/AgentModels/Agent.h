#pragma once
#include <memory>
#include <type_traits>
#include <typeinfo>
#include "OptiMA/AgentModels/AgentInfo.h"
#include "OptiMA/Engine/IAgentManager.h"
#include "OptiMA/Engine/PluginManager.h"
#include "OptiMA/Engine/Postmaster.h"
#include "OptiMA/Shared/Memory.h"
#include "OptiMA/Shared/SharedFactories.h"

namespace OptiMA
{
    class Agent
    {
    public:

        Agent();

        void start();

        void stop();

        void setAgentId(int agentId);

        void setAgentType(int agentType);

        void setCurrentTransaction(long transactionId);

        void setAgentManager(IAgentManager* amanger);

        void setPluginManager(PluginManager* pmanager);

        void setPostmaster(Postmaster* postmaster);

        void setSupervisors(vector<int>& supervisors);

        void setSubordinates(vector<int>& subordinates);

        void setCommunications(vector<int>& contacts);

        void setTools(vector<int>& tools);    

        AgentStatus getStatus();

        int getAgentId();

        int getAgentType();

        long getCurrentTransaction();

        PostBox* getPostBoxAddress();

        virtual void clearMemory() = 0;
        
        virtual ~Agent();

        friend class AgentManager;

    protected:

        IAgentManager* amanager_;        
        PluginManager* pmanager_;
        Postmaster* postmaster_;
        PostBox* postBox_;        
        vector<int> supervisors_;
        vector<int> subordinates_;
        vector<int> contacts_;
        vector<int> allowedPlugins_;
        long currentTransaction_;
        int agentId_;
        int agentType_;
        AgentStatus status_;        
        bool started_;        
    };
}