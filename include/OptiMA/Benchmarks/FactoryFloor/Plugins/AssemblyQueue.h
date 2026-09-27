#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class AssemblyQueue final : public Plugin<AssemblyQueue>
{ 
public:

    std::shared_ptr<Memory> operate(std::shared_ptr<Memory> inputParameters)
    {
        if(inputParameters == nullptr)
        {
            std::lock_guard<std::mutex> lock(queueLock_);
    
            if(jobQueue_.empty())
            {
                cooledDown = true;
                cooldownCondition.notify_one();
                return nullptr;
            }
        
            auto res = std::make_shared<Memory>();
            res->addTuple(std::move(jobQueue_.front()));
            jobQueue_.pop();
            started++;
            return res;
        }    
        else
        {
            std::shared_ptr<std::vector<std::unique_ptr<Job>>> jobVec = std::get<0>(inputParameters->getTuple<std::shared_ptr<std::vector<std::unique_ptr<Job>>>>(0));
            int count = jobVec->size();

            std::lock_guard<std::mutex> lock(queueLock_);

            for(int i = 0; i < count; i++)
            {
                jobQueue_.push(std::move((*jobVec)[i]));
            }
            
            return nullptr;        
        }
    }

private:

    std::queue<std::unique_ptr<Job>> jobQueue_;
    std::mutex queueLock_;
};