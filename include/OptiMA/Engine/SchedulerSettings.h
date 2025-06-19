#pragma once
#include "TxnSP/TxnSP.h"

namespace OptiMA
{
    struct SchedulerSettings
    {        
        double SA_DecrementParameter;
        double SA_MaxTemperature;        
        TxnSP::SolutionType DP_SolutionType;
        TxnSP::TemperatureEvolution SA_DecrementType;
        TxnSP::SolverType optimizationMethod;
        bool optimized;
        bool permuted;
    };
}