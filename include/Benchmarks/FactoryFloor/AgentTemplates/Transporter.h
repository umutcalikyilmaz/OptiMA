#pragma once
#include <chrono>
#include <thread>
#include <AgentModels/AgentTemplate.h>
#include <Benchmarks/FactoryFloor/FactoryFloorParameters.h>
#include <Benchmarks/FactoryFloor/Job.h>
#include <Benchmarks/Shared/RandomNumber.h>

using namespace OptiMA;

class Transporter : public AgentTemplate<Transporter>
{
    NormalRandom rnd;
    unique_ptr<Job> currentJob;
    double scale;
    int lastOperation;
    bool stopRequested;

public:

    Transporter() : rnd(transpoterTraverseMean * simulationTimeScale, transpoterTraverseStd * simulationTimeScale, randomNumberSeed),
    stopRequested(false) { }

    shared_ptr<Memory> PickUp()
    {
        lastOperation = 0;
        return OperatePlugin(1, nullptr);
    }

    shared_ptr<Memory> Traverse()
    {
        int duration = (int)rnd.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }

    shared_ptr<Memory> Place()
    {
        shared_ptr<Memory> input = GenerateMemory();
        input->addTuple(false);
        input->addTuple(move(currentJob));
        return OperatePlugin(3, input);
    }

    unique_ptr<ITransaction> BeginProcess(shared_ptr<Memory> initialParameters) override
    {
        set<int> plugins;
        plugins.insert(1);
        lastOperation = 0;
        return GenerateOperateTransaction(&Transporter::PickUp, plugins, 0);
    }

    unique_ptr<ITransaction> Process(shared_ptr<TransactionResult> previousResult) override
    {
        queue<shared_ptr<Message>> msgs = CheckMessages();

        if(!msgs.empty())
        {
            stopRequested = true;
        }

        set<int> plugins;

        switch (lastOperation)
        {
        case 0:
            if(previousResult->resultParameters == nullptr)
            {
                plugins.insert(1);
                return GenerateOperateTransaction(&Transporter::PickUp, plugins, 0);
            }

            currentJob = move(get<0>(previousResult->resultParameters->getTuple<unique_ptr<Job>>(0)));
            lastOperation = 1;
            return GenerateOperateTransaction(&Transporter::Traverse, plugins, 1);

        case 1:
            if(currentJob == nullptr)
            {
                if(stopRequested)
                {
                    stopRequested = false;
                    return nullptr;
                }

                plugins.insert(1);
                lastOperation = 0;
                return GenerateOperateTransaction(&Transporter::PickUp, plugins, 0);
            }
            else
            {
                plugins.insert(3);
                lastOperation = 2;
                return GenerateOperateTransaction(&Transporter::Place, plugins, 2);
            }

        case 2:
            lastOperation = 1;
            return GenerateOperateTransaction(&Transporter::Traverse, plugins, 1);
        
        default:
            return nullptr;
        }
    }

    void ClearMemory() override { }
};