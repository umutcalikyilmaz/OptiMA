#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <Benchmarks/FactoryFloor/Job.h>
#include <Benchmarks/Shared/RandomNumber.h>
#include <PluginModels/Plugin.h>

using namespace OptiMA;

class AssemblyQueue : public Plugin<AssemblyQueue>
{
    queue<unique_ptr<Job>> jobQueue;
    mutex queueLock;
    
public:

    shared_ptr<Memory> Operate(shared_ptr<Memory> inputParameters) override
    {
        if(inputParameters == nullptr)
        {
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
            shared_ptr<vector<unique_ptr<Job>>> jobVec = get<0>(inputParameters->getTuple<shared_ptr<vector<unique_ptr<Job>>>>(0));
            int count = jobVec->size();

            lock_guard<mutex> lock(queueLock);

            for(int i = 0; i < count; i++)
            {
                jobQueue.push(move((*jobVec)[i]));
            }
            
            return nullptr;        
        }
    }
};