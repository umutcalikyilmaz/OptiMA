#pragma once
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <Engine/IAgentManager.h>
#include <Engine/ITransactionFactory.h>
#include <Engine/PluginManager.h>
#include <Engine/Postmaster.h>
#include <Shared/SharedFactories.h>
#include <TransactionModels/ITransaction.h>
#include <TransactionModels/TransactionResult.h>

namespace OptiMA
{
    class Agent;
    template <typename A, typename... Args>
    class Operate;
    
    class AgentCore
    {
    protected:    
        
        ITransactionFactory* tfactory;
        vector<int> supervisors;
        vector<int> subordinates;
        vector<int> contacts;
        vector<int> allowedPlugins;
        bool started;

        virtual void SetAgentType(int agentType) = 0;        

        virtual void SetAgentManager(IAgentManager* amanger) = 0;

        virtual void SetTransactionFactory(ITransactionFactory* tfactory) = 0;

        virtual void SetPluginManager(PluginManager* pmanger) = 0;

        virtual void SetPostmaster(Postmaster* postmaster) = 0;

        virtual void SetPostBox(PostBox* postBox) = 0;

        virtual PostBox* GetPostBoxAddress() = 0;

        void SetSupervisors(vector<int>& supervisors);

        void SetSubordinates(vector<int>& subordinates);

        void SetCommunications(vector<int>& contacts);

        void SetTools(vector<int>& tools);        

    public:

        AgentCore();

        virtual void SetOwner(Agent* owner) = 0;

        unique_ptr<ITransaction> RequestBeginProcess(shared_ptr<Memory> initialParameters);

        unique_ptr<ITransaction> RequestProcess(shared_ptr<TransactionResult> previousResult);
        
        void Start();

        void Stop();

        virtual void SelfStop(int agentId, int agentType) = 0;
        
        virtual unique_ptr<ITransaction> BeginProcess(shared_ptr<Memory> initialParameters) = 0;

        virtual unique_ptr<ITransaction> Process(shared_ptr<TransactionResult> previousResult) = 0;

        virtual void ClearMemory() = 0;

        virtual ~AgentCore() = default;

        friend class CorePool;
        friend class AgentManager;
    };
}