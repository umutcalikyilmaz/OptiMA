#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class DrillPress : public Plugin<DrillPress>
{
    NormalRandom* operationRandoms_[2];
    UniformRandom probabilityRandom_;

public:

    DrillPress() : probabilityRandom_(0, 1, randomNumberSeed)
    {
        for(int i = 0; i < 2; i++)
        {
            operationRandoms_[i] = new NormalRandom(drillingOperationMeans[i] * simulationTimeScale, drillingOperationStds[i] * simulationTimeScale, randomNumberSeed * (6 + i));            
        }
    }

    shared_ptr<Memory> operate(shared_ptr<Memory> inputParameters) override
    {
        int operationType = get<0>(inputParameters->getTuple<int>(0));
        int duration = operationRandoms_[operationType]->generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }

    ~DrillPress()
    {
        delete operationRandoms_[0];
        delete operationRandoms_[1];
    }
};