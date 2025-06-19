#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/AgentModels/AgentTemplate.h"
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"

using namespace OptiMA;

class Inspector : public AgentTemplate<Inspector>
{
    NormalRandom reportRandom_;
    unique_ptr<Job> currentJob_;
    int supervisorId_;
    int lastOperation_;
    bool stopRequested_;

public:

    Inspector() : reportRandom_(inspectorReportMean * simulationTimeScale, inspectorReportStd * simulationTimeScale, randomNumberSeed),
    stopRequested_(false) { }

    shared_ptr<Memory> selfStop()
    {
        return stopAgent(getAgentId());
    }

    shared_ptr<Memory> pickUp()
    {
        shared_ptr<Memory> operationResult = operatePlugin(3, nullptr);
        shared_ptr<Memory> res = make_shared<Memory>();

        if(operationResult == nullptr)
        {
            res->addTuple(false);
        }
        else
        {
            currentJob_ = move(get<0>(operationResult->getTuple<unique_ptr<Job>>(0)));
            res->addTuple(true);
        }
        
        return res;
    }

    shared_ptr<Memory> scanPart()
    {
        shared_ptr<Memory> operationResult = operatePlugin(4, nullptr);
        currentJob_->isSuccessful = get<0>(operationResult->getTuple<bool>(0));
        return nullptr;
    }

    shared_ptr<Memory> reportResult()
    {
        int duration = (int)reportRandom_.generate();
        this_thread::sleep_for(chrono::milliseconds(duration));

        shared_ptr<Memory> res = make_shared<Memory>();

        auto msgs = checkMessages();
        res->addTuple(!msgs.empty());

        return res;
    }

    shared_ptr<Memory> place()
    {
        shared_ptr<Memory> input = generateMemory();
        input->addTuple(currentJob_->id, currentJob_->isSuccessful);
        shared_ptr<Memory> operationResult = operatePlugin(6, input);

        shared_ptr<Memory> res = generateMemory();
        res->addTuple(get<0>(operationResult->getTuple<bool>(0)));
        return res;
    }

    void clearMemory() override { }
};
