#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class ConveyorBelt : public Plugin<ConveyorBelt>
{
    NormalRandom rnd_;
    NormalRandom rnd2_;
    queue<unique_ptr<Job>> jobQueue_;
    mutex queueLock_;

public:

    ConveyorBelt() : rnd_(conveyorBeltPickUpMean * simulationTimeScale, conveyorBeltPickUpStd * simulationTimeScale, randomNumberSeed),
    rnd2_(conveyorBeltPlaceMean * simulationTimeScale, conveyorBeltPlaceStd * simulationTimeScale, randomNumberSeed) { }

    shared_ptr<Memory> operate(shared_ptr<Memory> inputParameters) override
    {
        if(inputParameters == nullptr)
        {
            queueLock_.lock();

            if(jobQueue_.empty())
            {
                queueLock_.unlock();
                return nullptr;
            }

            auto job = move(jobQueue_.front());
            jobQueue_.pop();
            queueLock_.unlock();
            
            int duration = (int)rnd_.generate();
            this_thread::sleep_for(chrono::milliseconds(duration));

            auto res = make_shared<Memory>();
            res->addTuple(move(job));
            return res;
        }
        else
        {
            bool type = get<0>(inputParameters->getTuple<bool>(0));
    
            if(type)
            {            
                auto res = generateMemory();
    
                lock_guard<mutex> lock(queueLock_);
                int count = jobQueue_.size();
                res->addTuple(count);
                return res;
            }
            else
            {
                int duration = (int)rnd2_.generate();
                this_thread::sleep_for(chrono::milliseconds(duration));
    
                lock_guard<mutex> lock(queueLock_);
                jobQueue_.push(move(get<0>(inputParameters->getTuple<unique_ptr<Job>>(1))));
            }        
    
            return nullptr;
        }
    }
};