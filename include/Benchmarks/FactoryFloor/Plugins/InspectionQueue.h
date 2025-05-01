#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <Benchmarks/FactoryFloor/FactoryFloorParameters.h>
#include <Benchmarks/FactoryFloor/Job.h>
#include <Benchmarks/Shared/RandomNumber.h>
#include <PluginModels/Plugin.h>

using namespace OptiMA;

class InspectionQueue : public Plugin<InspectionQueue>
{
    NormalRandom rnd;
    NormalRandom rnd2;
    queue<unique_ptr<Job>> jobQueue;
    mutex queueLock;

public:

    InspectionQueue() : rnd(inspectionQueuePickUpMean * simulationTimeScale, inspectionQueuePickUpStd * simulationTimeScale),
    rnd2(inspectionQueuePlaceMean * simulationTimeScale, inspectionQueuePlaceStd * simulationTimeScale, randomNumberSeed) { }

    shared_ptr<Memory> Operate(shared_ptr<Memory> inputParameters) override
    {
        if(inputParameters == nullptr)
        {
            int duration = (int)rnd.Generate();
            this_thread::sleep_for(chrono::milliseconds(duration));
    
            lock_guard<mutex> lock(queueLock);
    
            if(jobQueue.empty())
            {
                return nullptr;
            }

            auto res = make_shared<Memory>();
            res->addTuple(move(jobQueue.front()));
            jobQueue.pop();
            return res;
            
        }
        else
        {
            bool type = get<0>(inputParameters->getTuple<bool>(0));
    
            if(type)
            {
                auto res = GenerateMemory();
    
                lock_guard<mutex> lock(queueLock);
                int count = jobQueue.size();
                res->addTuple(count);
                return res;
            }
            else
            {
                int duration = (int)rnd2.Generate();
                this_thread::sleep_for(chrono::milliseconds(duration));
    
                lock_guard<mutex> lock(queueLock);                
                jobQueue.push(move(get<0>(inputParameters->getTuple<unique_ptr<Job>>(1))));
    
                return nullptr;
            }
            
        }
    }
};