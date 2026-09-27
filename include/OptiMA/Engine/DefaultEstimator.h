#pragma once
#include <fstream>
#include "OptiMA/Engine/Estimator.h"

namespace OptiMA
{
    class DefaultEstimator : public Estimator
    {
    public:
    
        DefaultEstimator(const std::string& statsFilePath);

        double estimateLength(const ITransaction& txn) override;

    private:

        std::map<int,std::map<int, double>> averages_;
        double generalAverage_;
    };
}