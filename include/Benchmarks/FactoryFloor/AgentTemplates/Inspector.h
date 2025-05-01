#pragma once
#include <chrono>
#include <thread>
#include <AgentModels/AgentTemplate.h>
#include <Benchmarks/FactoryFloor/FactoryFloorParameters.h>
#include <Benchmarks/FactoryFloor/Job.h>
#include <Benchmarks/Shared/RandomNumber.h>

using namespace OptiMA;

class Inspector : public AgentTemplate<Inspector>
{
    NormalRandom rnd;
    unique_ptr<Job> currentJob;
    int supervisorId;
    int lastOperation;
    bool stopRequested;

public:

    Inspector() : rnd(inspectorReportMean * simulationTimeScale, inspectorReportStd * simulationTimeScale, randomNumberSeed),
    stopRequested(false) { }

    shared_ptr<Memory> RetrievePart()
    {
        return OperatePlugin(3, nullptr);
    }

    shared_ptr<Memory> ScanPart()
    {
        return OperatePlugin(4, nullptr);
    }

    shared_ptr<Memory> ReportResult()
    {
        int duration = (int)rnd.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }

    unique_ptr<ITransaction> SendReport()
    {
        shared_ptr<Memory> parameters = GenerateMemory();
        parameters->addTuple(currentJob->id, currentJob->isSuccessful);
        auto msg = GenerateMessage(parameters);
        return GenerateCommunicateTransaction(msg, supervisorId);
    }

    unique_ptr<ITransaction> BeginProcess(shared_ptr<Memory> initialParameters) override
    {
        supervisorId = 0;
        set<int> plugins;
        plugins.insert(3);
        lastOperation = 0;
        return GenerateOperateTransaction(&Inspector::RetrievePart, plugins, 0);
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
                plugins.insert(3);
                return GenerateOperateTransaction(&Inspector::RetrievePart, plugins, 0);
            }

            currentJob = move(get<0>(previousResult->resultParameters->getTuple<unique_ptr<Job>>(0)));
            plugins.insert(4);
            lastOperation = 1;
            return GenerateOperateTransaction(&Inspector::ScanPart, plugins, 1);
        
        case 1:
            currentJob->isSuccessful = get<0>(previousResult->resultParameters->getTuple<bool>(0));
            lastOperation = 2;
            return GenerateOperateTransaction(&Inspector::ReportResult, plugins, 2);

        case 2:            
            lastOperation = 3;
            return SendReport();

        case 3:
            if(stopRequested)
            {
                stopRequested = false;
                return nullptr;
            }

            plugins.insert(3);
            lastOperation = 0;
            return GenerateOperateTransaction(&Inspector::RetrievePart, plugins, 0);
        
        default:
            return nullptr;
        }
    }

    void ClearMemory() override { }
};
