#pragma once
#include "OptiMA/AgentModels/AgentPool.h"
#include "OptiMA/Engine/IAgentManager.h"

namespace OptiMA
{
    class AgentManager final : public IAgentManager
    {
    public:

        class DriverKey
        {
        private:

            DriverKey() {}

            friend class Driver;
        };        

        AgentManager(DriverKey, const std::vector<std::unique_ptr<IAgentFactory>>& agentFactories,
            const std::vector<int>& agentTypes, const std::vector<int>& initialNumbers,
            const std::vector<int>& maxNumbers, const std::vector<std::pair<int,int>>& relationships,
            const std::vector<std::pair<int,int>>& communications,
            const std::vector<std::pair<int,int>>& pluginAccesses, const std::vector<int>& initialAgents,
            PluginManager* pmanager, long startingTime);        

        Agent* seizeAgent(TransactionKey, long transactionId, int agentType);

        void releaseAgent(TransactionKey, int agentId);

        void transferOwnership(TransactionKey, int transactionId, int agentId);

        void requestCreateAgent(AgentKey, long transactionId, int senderId, int senderType, int targetType) override;

        void requestCreateAndStartAgent(AgentKey, long transactionId, int senderId, int senderType, int targetType) override;

        void requestStartAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId) override;

        void requestStopAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId) override;

        void requestDestroyAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId) override;

        std::shared_ptr<Memory> getAgentInfo(AgentKey, int senderId, int senderType, int targetId) override;

        std::shared_ptr<Memory> getAgentInfos(AgentKey, int senderId, int senderType, int targetType) override;

        void commit(TransactionKey, long transactionId) override;

        void rollback(TransactionKey, long transactionId) override;

        void startInitialAgents(DriverKey);

        Postmaster* getPostmaster(DriverKey);

    private:

        std::unique_ptr<Postmaster> postmaster_;
        std::map<int, int> maxNumbers_;
        std::map<int, int> currentNumbers_;
        std::map<int, std::unique_ptr<AgentPool>> agentPools_;
        std::map<int, std::pair<std::unique_ptr<Agent>,int>> agentMap_;
        std::map<int, std::vector<int>> agentIds_;
        std::map<int, std::vector<int>> supervisors_;
        std::map<int, std::vector<int>> subordinates_;        
        std::map<int, std::vector<int>> tools_;
        std::map<int, std::unique_ptr<AgentInfo>> agentInfos_;
        std::vector<int> initialAgents_;
        std::map<long, std::vector<std::pair<AgentOperationType, int>>> transactionLog_;
        std::mutex agentLock_;
        std::mutex logLock_;
        long startingTime_;
        int agentCount_;

        bool checkSender(int senderType, int targetType);

        void enterLog(long transactionId, AgentOperationType operation, int parameter);

        void createAgent(int targetType);

        void createAndStartAgent(int targetType);

        void startAgent(int targetId);

        void stopAgent(int targetId);

        void destroyAgent(int targetId);
    };
}