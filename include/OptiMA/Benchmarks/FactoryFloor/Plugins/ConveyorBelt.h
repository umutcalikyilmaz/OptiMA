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

class ConveyorBelt final : public Plugin<ConveyorBelt>
{
public:

    ConveyorBelt()
        : rnd_(conveyorBeltPickUpMean * simulationTimeScale, conveyorBeltPickUpStd * simulationTimeScale, randomNumberSeed),
          rnd2_(conveyorBeltPlaceMean * simulationTimeScale, conveyorBeltPlaceStd * simulationTimeScale, randomNumberSeed) { }

    std::shared_ptr<Memory> operate(std::shared_ptr<Memory> inputParameters)
    {
        if(inputParameters == nullptr)
        {
            queueLock_.lock();

            if(jobQueue_.empty())
            {
                queueLock_.unlock();
                return nullptr;
            }

            auto job = std::move(jobQueue_.front());
            jobQueue_.pop();
            queueLock_.unlock();
            
            int duration = (int)rnd_.generate();
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));

            auto res = std::make_shared<Memory>();
            res->addTuple(move(job));
            return res;
        }
        else
        {
            bool type = get<0>(inputParameters->getTuple<bool>(0));
    
            if(type)
            {            
                auto res = generateMemory();
    
                std::lock_guard<std::mutex> lock(queueLock_);
                int count = jobQueue_.size();
                res->addTuple(count);
                return res;
            }
            else
            {
                int duration = (int)rnd2_.generate();
                std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    
                std::lock_guard<std::mutex> lock(queueLock_);
                jobQueue_.push(std::move(std::get<0>(inputParameters->getTuple<std::unique_ptr<Job>>(1))));
            }        
    
            return nullptr;
        }
    }

private:

    NormalRandom rnd_;
    NormalRandom rnd2_;
    std::queue<std::unique_ptr<Job>> jobQueue_;
    std::mutex queueLock_;
};