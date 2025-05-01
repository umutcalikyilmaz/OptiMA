#pragma once
#include <chrono>
#include <thread>
#include <Benchmarks/FactoryFloor/FactoryFloorParameters.h>
#include <Benchmarks/Shared/RandomNumber.h>
#include <PluginModels/Plugin.h>

using namespace OptiMA;

class WeldingStation : public Plugin<WeldingStation>
{
    NormalRandom rnd;

public:
    WeldingStation() : rnd(weldingStationOperationMean * simulationTimeScale, weldingStationOperationStd * simulationTimeScale, randomNumberSeed) { }

    shared_ptr<Memory> Operate(shared_ptr<Memory> inputParameters) override
    {
        int duration = (int)rnd.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }
};