#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class QAScanner : public Plugin<QAScanner>
{
    NormalRandom scanRandom_;
    UniformRandom probabilityRandom_;
    double successProbability = 0.9;

public:
    QAScanner() : scanRandom_(qaScannerOperationMean * simulationTimeScale, qaScannerOperationStd * simulationTimeScale, randomNumberSeed),
    probabilityRandom_(0, 1) { }

    shared_ptr<Memory> operate(shared_ptr<Memory> inputParameters) override
    {
        int duration = (int)scanRandom_.generate();
        this_thread::sleep_for(chrono::milliseconds(duration));

        shared_ptr<Memory> res = make_shared<Memory>();
        res->addTuple(probabilityRandom_.generate() < successProbability);
        return res;
    }
};