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

        SchedulerSettings() { };

        SchedulerSettings(const SchedulerSettings* org)
            : SA_DecrementParameter(org->SA_DecrementParameter),
              SA_MaxTemperature(org->SA_MaxTemperature),
              DP_SolutionType(org->DP_SolutionType),
              SA_DecrementType(org->SA_DecrementType),
              optimizationMethod(org->optimizationMethod),
              optimized(org->optimized),
              permuted(org->permuted) { }
    };
}