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

        virtual shared_ptr<TransactionResult> execute() = 0;

        virtual void setId(long transactionId) = 0; 

        virtual void setLength(double length) = 0;

        virtual void setDriver(IDriver* driver) = 0;

        virtual void setAgentManager(AgentManager* amanager) = 0;

        virtual void setPostMaster(Postmaster* postmaster) = 0;        

        virtual double getLength() const = 0;        

        virtual int getType() const = 0;

        virtual int getSubType() const = 0;

        virtual const vector<Agent*>& getSeizedAgents() const = 0;

        virtual const set<int>& getNonShareblePlugins() const = 0;

        //virtual void clear() = 0;

        virtual void findNonShareable(PluginManager* pmanager) = 0;

        virtual ~ITransaction() = default;
    };
}