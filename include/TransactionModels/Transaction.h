#pragma once
#include <AgentModels/Agent.h>
#include <Engine/PluginManager.h>
#include <Engine/Postmaster.h>
#include <PluginModels/Plugin.h>
#include <Shared/Exceptions.h>
#include <Shared/Memory.h>
#include <TransactionModels/ITransaction.h>
#include <TransactionModels/TransactionResult.h>

namespace OptiMA
{    
    class Transaction : public ITransaction
    {
        double length;
        long id;
        TransactionType type;
        bool processing;

    protected:        
        
        Agent* owner;
        shared_ptr<Memory> outputParameters;
        const set<int> nonShareablePlugins;
        int subType;

        void Abort();

        Transaction(Agent* owner, TransactionType type, int subType);

        Transaction(Agent* owner, TransactionType type, shared_ptr<Memory> outputParameters);

        Transaction(Agent* owner, TransactionType type, int subType, const set<int>& nonShareablePlugins);

    public:        

        virtual unique_ptr<ITransaction> Execute() override = 0;

        void SetLength(double length) override;

        double GetLength() override;

        const set<int>& GetNonShareblePlugins() override;

        TransactionType GetTransactionType() const override;

        int GetAgentType() const override;

        int GetSubType() const override;

        shared_ptr<Memory> GetOutputParameters() override;        

        virtual ~Transaction() = default;
    };
}