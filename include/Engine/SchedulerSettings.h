#pragma once
#include <TxnSPLib.h>

namespace OptiMA
{
    struct SchedulerSettings
    {        
        double SA_DecrementParameter;
        double SA_MaxTemperature;        
        TransactionScheduling::SolutionType DP_SolutionType;
        TransactionScheduling::TemperatureEvolution SA_DecrementType;
        TransactionScheduling::SolverType optimizationMethod;
        bool optimized;
        bool permuted;
    };
}