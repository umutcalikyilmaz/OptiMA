#pragma once
#include "OptiMA/AgentModels/AgentPool.h"
#include "OptiMA/Engine/IAgentManager.h"

namespace OptiMA
{
    class AgentManager final : public IAgentManager
    {
    public:

        AgentManager(vector<IAgentFactory*>& agentFactories, vector<int>& agentTypes, vector<int>& initialNumbers,
        vector<int>& maxNumbers, vector<pair<int,int>>& relationships, vector<pair<int,int>>& communications,
        vector<pair<int,int>> pluginAccesses, vector<int>& initialAgents, PluginManager* pmanager, long startingTime);        

        Agent* seizeAgent(long transactionId, int agentType);

        //void assignAgentById(long transactionId, int agentId);

        void releaseAgent(int agentId);

        void transferOwnership(int transactionId, int agentId);

        void requestCreateAgent(long transactionId, int senderId, int senderType, int targetType) override;

        void requestCreateAndStartAgent(long transactionId, int senderId, int senderType, int targetType) override;

        void requestStartAgent(long transactionId, int senderId, int senderType, int targetId) override;

        void requestStopAgent(long transactionId, int senderId, int senderType, int targetId) override;

        void requestDestroyAgent(long transactionId, int senderId, int senderType, int targetId) override;

        shared_ptr<Memory> getAgentInfo(int senderId, int senderType, int targetId) override;

        shared_ptr<Memory> getAgentInfos(int senderId, int senderType, int targetType) override;

        void commit(long transactionId) override;

        void rollback(long transactionId) override;

        void startInitialAgents();

        Postmaster* getPostmaster();

        ~AgentManager();

    private:

        Postmaster* postmaster_;
        map<int,int> maxNumbers_;
        map<int,int> currentNumbers_;
        map<int,AgentPool*> agentPools_;
        map<int,pair<Agent*,int>> agentMap_;
        map<int,vector<int>> agentIds_;
        map<int,vector<int>> supervisors_;
        map<int,vector<int>> subordinates_;        
        map<int,vector<int>> tools_;
        map<int,AgentInfo*> agentInfos_;
        vector<int> initialAgents_;
        map<long, vector<pair<AgentOperationType, int>>> transactionLog_;
        mutex agentLock_;
        mutex logLock_;
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