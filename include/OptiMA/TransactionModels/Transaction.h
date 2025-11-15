#pragma once
#include "OptiMA/PluginModels/Plugin.h"
#include "OptiMA/Shared/Exceptions.h"
#include "OptiMA/TransactionModels/ITransaction.h"

namespace OptiMA
{
    class Transaction : public ITransaction
    {
    private:

        IDriver* driver_;
        AgentManager* amanager_;
        Postmaster* postmaster_;
        vector<Agent*> seizedAgents_;
        const set<int> requestedPlugins_;
        set<int> nonShareablePlugins_;
        double length_;
        long id_;
        int type_;
        int subType_;

    protected:

        Transaction(int transactionType, int transactionSubType, const set<int>& requestedPlugins) : type_(transactionType), subType_(transactionSubType),
        requestedPlugins_(requestedPlugins) { }

        Transaction(const vector<Agent*>& agents, int transactionType, int transactionSubType, const set<int>& requestedPlugins) : seizedAgents_(agents),
        type_(transactionType), subType_(transactionSubType), requestedPlugins_(requestedPlugins) { }

        void abort()
        {
            throw UserAbortException("Aborted by the user");
        }

        void haltProgram(shared_ptr<Memory> outputParameters)
        {
            driver_->haltProgram(outputParameters);
        }

        Agent* seizeAgent(int agentType)
        {
            Agent* agent = amanager_->seizeAgent(id_, agentType);
            seizedAgents_.push_back(agent);
            return agent;
        }

        void releaseAgent(Agent* agent)
        {
            int agentId = agent->getAgentId();

            vector<Agent*>:: iterator it;
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
            amanager_->releaseAgent(agentId);
        }

        template<typename A, typename... IArgs>
        shared_ptr<Memory> executeInstruction(Agent* agent, shared_ptr<Memory>(A::*operation)(IArgs...), IArgs... arguments)
        {
            static_assert(is_base_of<AgentTemplate<decay_t<A>>, decay_t<A>>::value, "A member function of a child class of AgentTemplate is required");

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

            shared_ptr<Memory> output = (static_cast<A*>(agent)->*operation)(std::forward<IArgs>(arguments)...);
            return output;
        }

    public:

        virtual shared_ptr<Memory> procedure() = 0;

        virtual void commitProcedure() { }

        virtual void rollbackProcedure() { }

        shared_ptr<TransactionResult> execute() override final
        {
            shared_ptr<Memory> output;
            
            try
            {
                try
                {
                    output = procedure();
                }
                catch(UserAbortException e)
                {
                    rollback();
                    return make_shared<TransactionResult>(TransactionStatus::ABORTED, e.what());
                }
            }
            catch(exception& e)
            {
                rollback();
                return make_shared<TransactionResult>(TransactionStatus::FAILED, e.what());
            }        
            
            commit();
            return make_shared<TransactionResult>(TransactionStatus::SUCCESSFUL, output);
        }

        void commit()
        {
            amanager_->commit(id_);
            postmaster_->commit(id_);
            commitProcedure();
        }

        void rollback()
        {
            amanager_->rollback(id_);
            rollbackProcedure();
        }

        void setId(long transactionId) override
        {
            id_ = transactionId;
        }

        void setLength(double length) override
        {
            length_ = length;
        }

        void setDriver(IDriver* driver)
        {
            driver_ = driver;
        }

        void setAgentManager(AgentManager* amanager) override
        {
            amanager_ = amanager;

            for(Agent* agent : seizedAgents_)
            {
                amanager_->transferOwnership(id_, agent->getAgentId());
            }
        }
        
        void setPostMaster(Postmaster* postmaster) override
        {
            postmaster_ = postmaster;
        }

        double getLength() const override
        {
            return length_;
        }

        const set<int>& getNonShareblePlugins() const override
        {
            return nonShareablePlugins_;
        }

        const vector<Agent*>& getSeizedAgents() const override
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

        void findNonShareable(PluginManager* pmanager) override
        {
            nonShareablePlugins_ = pmanager->getNonShareable(requestedPlugins_);
            int asd = 0;
        }

        virtual ~Transaction() = default;
    };
}