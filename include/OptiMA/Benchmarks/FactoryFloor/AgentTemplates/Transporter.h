#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/AgentModels/AgentTemplate.h"
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"

using namespace OptiMA;

class Transporter : public AgentTemplate<Transporter>
{
    NormalRandom traverseRandom_;
    unique_ptr<Job> currentJob_;
    double scale_;
    int lastOperation_;
    bool stopRequested_;

public:

    Transporter() : traverseRandom_(transpoterTraverseMean * simulationTimeScale, transpoterTraverseStd * simulationTimeScale, randomNumberSeed),
    stopRequested_(false) { }

    shared_ptr<Memory> selfStop()
    {
        return stopAgent(getAgentId());
    }

    shared_ptr<Memory> pickUp()
    {
        auto msgs = checkMessages();

        if(!msgs.empty())
        {
            stopRequested_ = true;
        }

        shared_ptr<Memory> operationResult = operatePlugin(1, nullptr);
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

    shared_ptr<Memory> traverse()
    {
        int duration = (int)traverseRandom_.generate();
        this_thread::sleep_for(chrono::milliseconds(duration));

        shared_ptr<Memory> res = make_shared<Memory>();

        auto msgs = checkMessages();
        res->addTuple(!msgs.empty());

        return res;
    }

    shared_ptr<Memory> place()
    {
        shared_ptr<Memory> input = generateMemory();
        input->addTuple(false);
        input->addTuple(move(currentJob_));
        return operatePlugin(3, input);
    }

    void clearMemory() override { }
};