#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/AgentModels/AgentTemplate.h"
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"

using namespace OptiMA;

class Inspector final : public AgentTemplate<Inspector>
{
public:

    Inspector()
        : reportRandom_(inspectorReportMean * simulationTimeScale, inspectorReportStd * simulationTimeScale, randomNumberSeed),
          stopRequested_(false) { }

    std::shared_ptr<Memory> selfStop()
    {
        return stopAgent(getAgentId());
    }

    std::shared_ptr<Memory> pickUp()
    {
        std::shared_ptr<Memory> operationResult = operatePlugin(3, nullptr);
        std::shared_ptr<Memory> res = std::make_shared<Memory>();

        if(operationResult == nullptr)
        {
            res->addTuple(false);
        }
        else
        {
            currentJob_ = std::move(std::get<0>(operationResult->getTuple<std::unique_ptr<Job>>(0)));
            res->addTuple(true);
        }
        
        return res;
    }

    std::shared_ptr<Memory> scanPart()
    {
        std::shared_ptr<Memory> operationResult = operatePlugin(4, nullptr);
        currentJob_->isSuccessful = get<0>(operationResult->getTuple<bool>(0));
        return nullptr;
    }

    std::shared_ptr<Memory> reportResult()
    {
        int duration = (int)reportRandom_.generate();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));

        std::shared_ptr<Memory> res = std::make_shared<Memory>();

        auto msgs = checkMessages();
        res->addTuple(!msgs.empty());

        return res;
    }

    std::shared_ptr<Memory> place()
    {
        std::shared_ptr<Memory> input = generateMemory();
        input->addTuple(currentJob_->id, currentJob_->isSuccessful);
        std::shared_ptr<Memory> operationResult = operatePlugin(6, input);

        std::shared_ptr<Memory> res = generateMemory();
        res->addTuple(get<0>(operationResult->getTuple<bool>(0)));
        return res;
    }

    void clearMemory() override { }

private: 

    NormalRandom reportRandom_;
    std::unique_ptr<Job> currentJob_;
    int supervisorId_;
    int lastOperation_;
    bool stopRequested_;
};
