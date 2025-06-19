#pragma once
#include <mutex>
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class OutputBin : public Plugin<OutputBin>
{
private:

    vector<pair<int,bool>> completedJobs_;
    mutex binLock_;
    int totalJobNum_;

public:

    OutputBin() : totalJobNum_(totalJobNumber) { }

    shared_ptr<Memory> operate(shared_ptr<Memory> inputParameters) override
    {
        lock_guard<mutex> lock(binLock_);
        auto inputTuple = inputParameters->getTuple<int,bool>(0);
        completedJobs_.push_back(make_pair(get<0>(inputTuple), get<1>(inputTuple)));
        
        shared_ptr<Memory> res = generateMemory();
        res->addTuple(completedJobs_.size() == totalJobNum_);
        return res;
    }
};