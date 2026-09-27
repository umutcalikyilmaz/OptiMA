#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class QAScanner final : public Plugin<QAScanner>
{
public:

    QAScanner()
        : scanRandom_(qaScannerOperationMean * simulationTimeScale, qaScannerOperationStd * simulationTimeScale, randomNumberSeed),
          probabilityRandom_(0, 1) { }

    std::shared_ptr<Memory> operate(std::shared_ptr<Memory> inputParameters)
    {
        int duration = (int)scanRandom_.generate();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));

        std::shared_ptr<Memory> res = std::make_shared<Memory>();
        res->addTuple(probabilityRandom_.generate() < successProbability);
        return res;
    }

private:

    NormalRandom scanRandom_;
    UniformRandom probabilityRandom_;
    double successProbability = 0.9;
};