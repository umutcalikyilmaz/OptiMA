#pragma once
#include <TransactionModels/TransactionQueue.h>

namespace OptiMA
{
    class IScheduler
    {
    protected:

        virtual ~IScheduler() = default;

        friend class Listener;
    };
}