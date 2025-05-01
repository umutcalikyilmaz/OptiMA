#pragma once
#include <TransactionModels/Transaction.h>

namespace OptiMA
{
    class Estimator
    {
    public:

        virtual double EstimateLength(const ITransaction& txn) = 0;
    };
}