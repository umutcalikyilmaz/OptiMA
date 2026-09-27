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

        class ManagerKey
        {
        private:

            ManagerKey() {}

            friend class AgentManager;
        };

        class PoolKey
        {
        private:
            
            PoolKey() {}

            friend class AgentPool;
        };

        void start(ManagerKey);

        void stop(ManagerKey);

        void setAgentId(ManagerKey, int agentId);        

        void setCurrentTransaction(ManagerKey, long transactionId);

        AgentStatus getStatus(ManagerKey);

        PostBox* getPostBoxAddress(ManagerKey);

        void setAgentType(PoolKey, int agentType);

        void setCurrentTransaction(PoolKey);

        void setAgentManager(PoolKey, IAgentManager* amanger);

        void setPluginManager(PoolKey, PluginManager* pmanager);

        void setPostmaster(PoolKey, Postmaster* postmaster);

        void setSupervisors(PoolKey, const std::vector<int>& supervisors);

        void setSubordinates(PoolKey, const std::vector<int>& subordinates);

        void setCommunications(PoolKey, const std::vector<int>& contacts);

        void setTools(PoolKey, const std::vector<int>& tools);    

        void clearMemory(PoolKey);

        int getAgentId();

        //int getAgentType();

        //long getCurrentTransaction();

        virtual ~Agent() = default;

    protected:

        IAgentManager* amanager_;        
        PluginManager* pmanager_;
        Postmaster* postmaster_;
        std::unique_ptr<PostBox> postBox_;        
        std::vector<int> supervisors_;
        std::vector<int> subordinates_;
        std::vector<int> contacts_;
        std::vector<int> allowedPlugins_;
        long currentTransaction_;
        int agentId_;
        int agentType_;
        AgentStatus status_;        
        bool started_;
        
        Agent();

        virtual void clearMemory() = 0;
    };
}