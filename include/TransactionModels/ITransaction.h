#pragma once
#include <Shared/Memory.h>

namespace OptiMA
{
    class ITransaction
    {
    public:
    
        virtual unique_ptr<ITransaction> Execute() = 0;

        virtual void SetLength(double length) = 0;

        virtual double GetLength() = 0;

        virtual const set<int>& GetNonShareblePlugins() = 0;

        virtual TransactionType GetTransactionType() const = 0;

        virtual int GetAgentType() const = 0;

        virtual int GetSubType() const = 0;

        virtual shared_ptr<Memory> GetOutputParameters() = 0;

        virtual ~ITransaction() = default;
    };
}