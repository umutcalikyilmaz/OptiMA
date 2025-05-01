#pragma once
#include <AgentModels/AgentCore.h>

namespace OptiMA
{
    template <class A> class AgentTemplate : public AgentCore
    {
        Agent* owner;
        IAgentManager* amanager;        
        PluginManager* pmanager;
        Postmaster* postmaster;
        PostBox* postBox;
        int agentType;

        void SetAgentType(int agentType)
        {
            this->agentType = agentType;
        }

        void SetAgentManager(IAgentManager* amanager) override final
        {
            this->amanager = amanager;
        }

        void SetTransactionFactory(ITransactionFactory* tfactory) override final
        {
            this->tfactory = tfactory;
        }

        void SetPluginManager(PluginManager* pmanager) override final
        {
            this->pmanager = pmanager;
        }

        void SetPostmaster(Postmaster* postmaster) override final
        {
            this->postmaster = postmaster;
        }

        void SetPostBox(PostBox* postBox) override final
        {
            this->postBox = postBox;
        }

        PostBox* GetPostBoxAddress() override final
        {
            return postBox;
        }

    protected:

        shared_ptr<Memory> OperatePlugin(int pluginId, shared_ptr<Memory> input)
        {
            PluginInstance* pi = pmanager->SeizePlugin(pluginId, agentType);
            auto res = pi->Operate(input);
            pmanager->ReleasePlugin(pi);
            return res;
        }
    
        template <typename... Args>
        unique_ptr<ITransaction> GenerateOperateTransaction(shared_ptr<Memory>(A::*operateFunction)(Args...),
        set<int>& requestedPlugins, int subType, Args... arguments)
        {
            set<int> nonShareablePlugins = pmanager->GetNonShareable(requestedPlugins);
            
            return make_unique<Operate<A,Args...>>(owner, nonShareablePlugins, static_cast<A*>(this), operateFunction, subType,
            arguments...);            
        }

        queue<shared_ptr<Message>> CheckMessages()
        {
            return postBox->CheckMessages();
        }

        unique_ptr<ITransaction> GenerateCommunicateTransaction(shared_ptr<Message> msg, int receiverId)
        {
            return tfactory->GenerateCommunicateTransaction(owner, postmaster, msg, receiverId);
        }

        unique_ptr<ITransaction> GenerateCommunicateTransaction(shared_ptr<Message> msg, int receiverIdOrType, bool toAll)
        {
            return tfactory->GenerateCommunicateTransaction(owner, postmaster, msg, receiverIdOrType, toAll);
        }

        unique_ptr<ITransaction> GenerateCreateAgentTransaction(int targetType)
        {
            return tfactory->GenerateCreateAgentTransaction(owner, amanager, targetType);
        }

        unique_ptr<ITransaction> GenerateCreateAgentTransaction(int targetType, shared_ptr<Memory> initialParameters)
        {
            return tfactory->GenerateCreateAgentTransaction(owner, amanager, targetType, initialParameters);
        }

        unique_ptr<ITransaction> GenerateDestroyAgentTransaction(int targetId)
        {
            return tfactory->GenerateDestroyAgentTransaction(owner, amanager, targetId);
        }

        unique_ptr<ITransaction> GenerateGetAgentInfoTransaction(int targetId)
        {
            return tfactory->GenerateGetAgentInfoTransaction(owner, amanager, targetId);
        }

        unique_ptr<ITransaction> GenerateGetAgentInfoTransaction(int targetIdOrType, bool fromAll)
        {
            return tfactory->GenerateGetAgentInfoTransaction(owner, amanager, targetIdOrType, fromAll);
        }

        unique_ptr<ITransaction> GenerateHaltProgramTransaction(shared_ptr<Memory> outputParameters)
        {
            return tfactory->GenerateHaltProgramTransaction(owner, outputParameters);
            
        }

        unique_ptr<ITransaction> GenerateStartAgentTransaction(int targetId, shared_ptr<Memory> initialParameters)
        {
            return tfactory->GenerateStartAgentTransaction(owner, amanager, targetId, initialParameters);
        }

        unique_ptr<ITransaction> GenerateStopAgentTransaction(int targetId)
        {
            return tfactory->GenerateStopAgentTransaction(owner, amanager, targetId);
        }

        AgentTemplate() : postBox(new PostBox()) { }

    public:
    
        void SetOwner(Agent* owner) override final
        {
            this->owner = owner;
        }

        void ClearMemory()
        {
            static_cast<A*>(this)->ClearMemory();
        }

        virtual unique_ptr<ITransaction> BeginProcess(shared_ptr<Memory> initialParameters) = 0;

        virtual unique_ptr<ITransaction> Process(shared_ptr<TransactionResult> previousResult) = 0;

        void SelfStop(int agentId, int agentType) override final
        {
            amanager->StopAgent(agentId, agentType, agentId);
        }

        virtual ~AgentTemplate()
        {
            delete postBox;
        }
    };
}