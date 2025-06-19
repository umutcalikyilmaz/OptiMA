#pragma once
#include "OptiMA/TransactionModels/ITransaction.h"

namespace OptiMA
{
    class Estimator
    {
    public:

        virtual double estimateLength(const ITransaction& txn) = 0;
    };
}