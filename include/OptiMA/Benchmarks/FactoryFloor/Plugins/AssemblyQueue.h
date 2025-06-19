#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"
#include "OptiMA/PluginModels/Plugin.h"

using namespace OptiMA;

class AssemblyQueue : public Plugin<AssemblyQueue>
{
    queue<unique_ptr<Job>> jobQueue_;
    mutex queueLock_;
    
public:

    shared_ptr<Memory> operate(shared_ptr<Memory> inputParameters) override
    {
        if(inputParameters == nullptr)
        {
            lock_guard<mutex> lock(queueLock_);
    
            if(jobQueue_.empty())
            {
                return nullptr;
            }
        
            auto res = make_shared<Memory>();
            res->addTuple(move(jobQueue_.front()));
            jobQueue_.pop();
            return res;
        }    
        else
        {
            shared_ptr<vector<unique_ptr<Job>>> jobVec = get<0>(inputParameters->getTuple<shared_ptr<vector<unique_ptr<Job>>>>(0));
            int count = jobVec->size();

            lock_guard<mutex> lock(queueLock_);

            for(int i = 0; i < count; i++)
            {
                jobQueue_.push(move((*jobVec)[i]));
            }
            
            return nullptr;        
        }
    }
};