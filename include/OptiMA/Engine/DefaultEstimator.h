#pragma once
#include <fstream>
#include "OptiMA/Engine/Estimator.h"

namespace OptiMA
{
    class DefaultEstimator : public Estimator
    {
    public:
    
        DefaultEstimator(string statsFilePath);

        double estimateLength(const ITransaction& txn) override;

    private:

        map<int,map<int,double>> averages_;
        double generalAverage_;
    };
}