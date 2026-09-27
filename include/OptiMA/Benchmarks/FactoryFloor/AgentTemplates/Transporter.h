#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/AgentModels/AgentTemplate.h"
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"

using namespace OptiMA;

class Transporter final : public AgentTemplate<Transporter>
{
public:

    Transporter()
        : traverseRandom_(transpoterTraverseMean * simulationTimeScale, transpoterTraverseStd * simulationTimeScale, randomNumberSeed),
          stopRequested_(false) { }

    std::shared_ptr<Memory> selfStop()
    {
        return stopAgent(getAgentId());
    }

    std::shared_ptr<Memory> pickUp()
    {
        auto msgs = checkMessages();

        if(!msgs.empty())
        {
            stopRequested_ = true;
        }

        std::shared_ptr<Memory> operationResult = operatePlugin(1, nullptr);
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

    std::shared_ptr<Memory> traverse()
    {
        int duration = (int)traverseRandom_.generate();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));

        std::shared_ptr<Memory> res = std::make_shared<Memory>();

        auto msgs = checkMessages();
        res->addTuple(!msgs.empty());

        return res;
    }

    std::shared_ptr<Memory> place()
    {
        std::shared_ptr<Memory> input = generateMemory();
        input->addTuple(false);
        input->addTuple(move(currentJob_));
        return operatePlugin(3, input);
    }

    void clearMemory() override { }

private:

    NormalRandom traverseRandom_;
    std::unique_ptr<Job> currentJob_;
    double scale_;
    int lastOperation_;
    bool stopRequested_;
};