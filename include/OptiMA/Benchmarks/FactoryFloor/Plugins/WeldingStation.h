#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class WeldingStation final : public Plugin<WeldingStation>
{
public:
    WeldingStation() : probabilityRandom_(0, 1, randomNumberSeed)
    {
        for(int i = 0; i < 2; i++)
        {
            operationRandoms_[i] = std::make_unique<NormalRandom>(weldingOperationMeans[i] * simulationTimeScale,
                weldingOperationStds[i] * simulationTimeScale, randomNumberSeed * (8 + i));            
        }
    }

    std::shared_ptr<Memory> operate(std::shared_ptr<Memory> inputParameters)
    {
        int operationType = get<0>(inputParameters->getTuple<int>(0));
        int duration = operationRandoms_[operationType]->generate();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
        return nullptr;
    }

private:

    std::unique_ptr<NormalRandom> operationRandoms_[2];
    UniformRandom probabilityRandom_;
};