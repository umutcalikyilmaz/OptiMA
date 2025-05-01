#pragma once
#include <chrono>
#include <thread>
#include <Benchmarks/FactoryFloor/FactoryFloorParameters.h>
#include <Benchmarks/Shared/RandomNumber.h>
#include <PluginModels/Plugin.h>

using namespace OptiMA;

class DrillPress : public Plugin<DrillPress>
{
    NormalRandom rnd;

public:

    DrillPress() : rnd(drillPressOperationMean * simulationTimeScale, drillPressOperationStd * simulationTimeScale, randomNumberSeed) { }

    shared_ptr<Memory> Operate(shared_ptr<Memory> inputParameters) override
    {
        int duration = (int)rnd.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }
};