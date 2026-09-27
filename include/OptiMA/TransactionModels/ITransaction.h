#pragma once
#include "OptiMA/Engine/AgentManager.h"
#include "OptiMA/Engine/IDriver.h"
#include "OptiMA/Engine/Postmaster.h"
#include "OptiMA/Shared/Memory.h"
#include "OptiMA/TransactionModels/TransactionResult.h"

namespace OptiMA
{
    class ITransaction
    {
    public:

        class ExecutorKey
        {
        private:

            ExecutorKey() {}

            friend class Executor;
        };

        class ListenerKey
        {
        private:
            
            ListenerKey() {}

            friend class Listener;
        };

        virtual std::shared_ptr<TransactionResult> execute(ExecutorKey) = 0;

        virtual void setId(ListenerKey, long transactionId) = 0; 

        virtual void setLength(ListenerKey, double length) = 0;

        virtual void setDriver(ListenerKey, IDriver* driver) = 0;

        virtual void setAgentManager(ListenerKey, AgentManager* amanager) = 0;

        virtual void setPostmaster(ListenerKey, Postmaster* postmaster) = 0; 
        
        virtual void findNonShareable(ListenerKey, PluginManager* pmanager) = 0;

        virtual double getLength() const = 0;        

        virtual int getType() const = 0;

        virtual int getSubType() const = 0;

        virtual const std::vector<Agent*>& getSeizedAgents() const = 0;

        virtual const std::set<int>& getNonShareablePlugins() const = 0;        

        virtual ~ITransaction() = default;
    };
}