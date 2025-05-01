#pragma once
#include <Shared/SharedParameters.h>

namespace TransactionScheduling
{
    struct AnalyzerInput
    {
        string directory;
        double cpStepSize;
        double para1;
        double para2;
        int n;
        int m;
        int prbNum;
        int threadCount;
        ProbabilityDistribution dist;
    };
    
}