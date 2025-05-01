#pragma once
#include <chrono>
#include <thread>
#include <Benchmarks/FactoryFloor/FactoryFloorParameters.h>
#include <Benchmarks/FactoryFloor/Job.h>
#include <Benchmarks/Shared/RandomNumber.h>
#include <PluginModels/Plugin.h>

using namespace OptiMA;

class QAScanner : public Plugin<QAScanner>
{
    NormalRandom rnd;
    UniformRandom rnd2;
    double successProbability = 0.9;

public:
    QAScanner() : rnd(qaScannerOperationMean * simulationTimeScale, qaScannerOperationStd * simulationTimeScale, randomNumberSeed),
    rnd2(0, 1) { }

    shared_ptr<Memory> Operate(shared_ptr<Memory> inputParameters) override
    {
        int duration = (int)rnd.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));

        shared_ptr<Memory> res = make_shared<Memory>();    
        double p = rnd2.Generate();
    
        if(p < successProbability)
        {
            res->addTuple(true);
        }
        else
        {
            res->addTuple(false);
        }
    
        return res;
    }
};