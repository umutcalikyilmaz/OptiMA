#pragma once
#include <fstream>
#include <Engine/Estimator.h>

namespace OptiMA
{
    class DefaultEstimator : public Estimator
    {
        map<int,map<int,map<int,double>>> averages;

    public:
    
        DefaultEstimator(string statsFilePath);

        double EstimateLength(const ITransaction& txn) override;
    };
}