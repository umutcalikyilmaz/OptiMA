#pragma once
#include <AgentModels/Agent.h>
#include <AgentModels/CorePool.h>
#include <Engine/IAgentManager.h>
#include <Engine/Listener.h>
#include <Engine/TransactionFactory.h>
#include <TransactionModels/Begin.h>

namespace OptiMA
{
    class AgentManager final : public IAgentManager
    {
        Listener* listener;
        Postmaster* postmaster;
        ITransactionFactory* tfactory;
        map<int,int> maxNumbers;
        map<int,int> currentNumbers;
        map<int,CorePool*> corePools;
        map<int,pair<Agent*,int>> agentMap;
        map<int,vector<int>> agentIds;
        map<int,vector<int>> supervisors;
        map<int,vector<int>> subordinates;        
        map<int,vector<int>> tools;
        map<int,AgentInfo*> agentInfos;
        vector<int> initialAgents;
        map<int,shared_ptr<Memory>> initialParameters;
        long startingTime;
        int agentCount;
        mutex agentLock;

        bool CheckSender(int senderType, int targetType);

        void SendBeginTransaction(Agent* owner, shared_ptr<Memory> initialParameters);

    public:

        AgentManager(vector<IAgentCoreFactory*>& coreFactories, vector<int>& coreIds, vector<int>& initialNumbers,
        vector<int>& maxNumbers, vector<pair<int,int>>& relationships, vector<pair<int,int>>& communications,
        vector<pair<int,int>> pluginAccesses, vector<int>& initialAgents, map<int,shared_ptr<Memory>>& initialParameters,
        PluginManager* pmanager, Listener* listener, long startingTime);        

        void CreateAgent(int senderId, int senderType, int targetType) override;

        void CreateAgent(int senderId, int senderType, int targetType, shared_ptr<Memory> initialParameters) override;

        void StartAgent(int senderId, int senderType, int targetId, shared_ptr<Memory> initialParameters) override;

        void StopAgent(int senderId, int senderType, int targetId) override;

        void DestroyAgent(int senderId, int senderType, int targetId) override;

        shared_ptr<Memory> GetAgentInfo(int senderId, int senderType, int targetId) override;

        shared_ptr<Memory> GetAgentInfos(int senderId, int senderType, int targetType) override;

        void StartInitialAgents();

        ~AgentManager();
    };
}