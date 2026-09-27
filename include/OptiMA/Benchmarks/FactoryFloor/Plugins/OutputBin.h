#pragma once
#include <mutex>
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class OutputBin final : public Plugin<OutputBin>
{
public:

    OutputBin()
        : totalJobNum_(totalJobNumber) { }

    std::shared_ptr<Memory> operate(std::shared_ptr<Memory> inputParameters)
    {
        std::lock_guard<std::mutex> lock(binLock_);
        auto inputTuple = inputParameters->getTuple<int,bool>(0);
        completedJobs_.push_back(std::make_pair(std::get<0>(inputTuple), std::get<1>(inputTuple)));
        
        std::shared_ptr<Memory> res = generateMemory();
        res->addTuple(completedJobs_.size() == totalJobNum_);
        completed++;
        warmedUp = true;
        warmupCondition.notify_one();
        return res;
    }

private:

    std::vector<std::pair<int,bool>> completedJobs_;
    std::mutex binLock_;
    int totalJobNum_;
};