#pragma once
#include "OptiMA/PluginModels/Plugin.h"
#include "OptiMA/Shared/Exceptions.h"
#include "OptiMA/TransactionModels/ITransaction.h"

namespace OptiMA
{
    class Transaction : public ITransaction
    {
    public:

        std::shared_ptr<TransactionResult> execute(ExecutorKey) override final
        {
            std::shared_ptr<Memory> output;
            
            try
            {
                try
                {
                    output = procedure();
                }
                catch(UserAbortException e)
                {
                    rollback();
                    return std::make_shared<TransactionResult>(TransactionStatus::ABORTED, e.what());
                }
            }
            catch(std::exception& e)
            {
                rollback();
                return std::make_shared<TransactionResult>(TransactionStatus::FAILED, e.what());
            }        
            
            commit();
            return std::make_shared<TransactionResult>(TransactionStatus::SUCCESSFUL, output);
        }

        void setId(ListenerKey, long transactionId) override
        {
            id_ = transactionId;
        }

        void setLength(ListenerKey, double length) override
        {
            length_ = length;
        }

        void setDriver(ListenerKey, IDriver* driver)
        {
            driver_ = driver;
        }

        void setAgentManager(ListenerKey, AgentManager* amanager) override
        {
            amanager_ = amanager;

            for(Agent* agent : seizedAgents_)
            {
                amanager_->transferOwnership(typename IAgentManager::TransactionKey{}, id_,
                    agent->getAgentId());
            }
        }
        
        void setPostmaster(ListenerKey, Postmaster* postmaster) override
        {
            postmaster_ = postmaster;
        }

        void findNonShareable(ListenerKey, PluginManager* pmanager) override
        {
            nonShareablePlugins_ = pmanager->getNonShareable(requestedPlugins_);
        }

        double getLength() const override
        {
            return length_;
        }

        const std::set<int>& getNonShareablePlugins() const override
        {
            return nonShareablePlugins_;
        }

        const std::vector<Agent*>& getSeizedAgents() const override
        {
            return seizedAgents_;
        }

        int getType() const override
        {
            return type_;
        }

        int getSubType() const override
        {
            return subType_;
        }

        virtual ~Transaction() = default;

    protected:

        Transaction(int transactionType, int transactionSubType, const std::set<int>& requestedPlugins)
            : type_(transactionType),
              subType_(transactionSubType),
              requestedPlugins_(requestedPlugins) { }

        Transaction(const std::vector<Agent*>& agents, int transactionType, int transactionSubType,
            const std::set<int>& requestedPlugins)
            : seizedAgents_(agents),
              type_(transactionType),
              subType_(transactionSubType),
              requestedPlugins_(requestedPlugins) { }

        void abort()
        {
            throw UserAbortException("Aborted by the user");
        }

        void haltProgram(std::shared_ptr<Memory> outputParameters)
        {
            driver_->haltProgram(outputParameters);
        }

        Agent* seizeAgent(int agentType)
        {
            Agent* agent = amanager_->seizeAgent(typename IAgentManager::TransactionKey{}, id_, agentType);
            seizedAgents_.push_back(agent);
            return agent;
        }

        void releaseAgent(Agent* agent)
        {
            int agentId = agent->getAgentId();

            std::vector<Agent*>:: iterator it;
            bool found = false;

            for(it = seizedAgents_.begin(); it != seizedAgents_.end(); it++)
            {
                if((*it)->getAgentId() == agentId)
                {
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                throw AgentUnavailableException("The agent is not seized by this transaction");
            }

            seizedAgents_.erase(it);
            amanager_->releaseAgent(typename IAgentManager::TransactionKey{}, agentId);
        }

        template<typename A, typename... IArgs>
        std::shared_ptr<Memory> executeInstruction(Agent* agent, std::shared_ptr<Memory>(A::*operation)(IArgs...), IArgs... arguments)
        {
            static_assert(std::is_base_of<AgentTemplate<std::decay_t<A>>, std::decay_t<A>>::value, "A member function of a child class of AgentTemplate is required");

            bool found = false;
            int agentId = agent->getAgentId();

            for(Agent* a : seizedAgents_)
            {
                if(a->getAgentId() == agentId)
                {
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                throw UnautorizedAccessException("The agent is not seized by this transaction");
            }

            std::shared_ptr<Memory> output = (static_cast<A*>(agent)->*operation)(std::forward<IArgs>(arguments)...);
            return output;
        }

    private:

        IDriver* driver_;
        AgentManager* amanager_;
        Postmaster* postmaster_;
        std::vector<Agent*> seizedAgents_;
        const std::set<int> requestedPlugins_;
        std::set<int> nonShareablePlugins_;
        double length_;
        long id_;
        int type_;
        int subType_;

        virtual void commitProcedure() { }

        virtual void rollbackProcedure() { }

        virtual std::shared_ptr<Memory> procedure() = 0;

        void commit()
        {
            amanager_->commit(typename IAgentManager::TransactionKey {}, id_);
            postmaster_->commit(typename Postmaster::TransactionKey {}, id_);
            commitProcedure();
        }

        void rollback()
        {
            amanager_->rollback(typename IAgentManager::TransactionKey {}, id_);
            postmaster_->rollback(typename Postmaster::TransactionKey {}, id_);
            rollbackProcedure();
        }
    };
}