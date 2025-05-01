#pragma once
#include <chrono>
#include <thread>
#include <AgentModels/AgentTemplate.h>
#include <Benchmarks/FactoryFloor/FactoryFloorParameters.h>
#include <Benchmarks/FactoryFloor/Job.h>
#include <Benchmarks/Shared/RandomNumber.h>

using namespace OptiMA;

class AssemblyWorker : public AgentTemplate<AssemblyWorker>
{
    unique_ptr<Job> currentJob;
    NormalRandom rnd1;
    NormalRandom rnd2;
    NormalRandom rnd3;
    NormalRandom rnd4;
    NormalRandom rnd5;
    double timeScale;
    bool stopRequested;

public:

    AssemblyWorker() : rnd1(assemblyManualOperationMeans[0] * simulationTimeScale, assemblyManualOperationStds[0] * simulationTimeScale, randomNumberSeed),
    rnd2(assemblyManualOperationMeans[1] * simulationTimeScale, assemblyManualOperationStds[1] * simulationTimeScale, randomNumberSeed),
    rnd3(assemblyManualOperationMeans[2] * simulationTimeScale, assemblyManualOperationStds[2] * simulationTimeScale, randomNumberSeed),
    rnd4(assemblyManualOperationMeans[3] * simulationTimeScale, assemblyManualOperationStds[3] * simulationTimeScale, randomNumberSeed),
    rnd5(assemblyManualOperationMeans[4] * simulationTimeScale, assemblyManualOperationStds[4] * simulationTimeScale, randomNumberSeed),    
    currentJob(nullptr), stopRequested(false) { }

    shared_ptr<Memory> RetrieveJob()
    {
        return OperatePlugin(0, nullptr);
    }

    shared_ptr<Memory> ManualOperation1()
    {
        int duration = (int)rnd1.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }

    shared_ptr<Memory> ManualOperation2()
    {
        int duration = (int)rnd2.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }

    shared_ptr<Memory> ManualOperation3()
    {
        int duration = (int)rnd3.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }

    shared_ptr<Memory> ManualOperation4()
    {
        int duration = (int)rnd4.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }

    shared_ptr<Memory> ManualOperation5()
    {
        int duration = (int)rnd5.Generate();
        this_thread::sleep_for(chrono::milliseconds(duration));
        return nullptr;
    }

    shared_ptr<Memory> OperateDrillPress()
    {
        return OperatePlugin(2, nullptr);
    }

    shared_ptr<Memory> OperateWeldingStation()
    {
        return OperatePlugin(5, nullptr);
    }
  
    shared_ptr<Memory> PlaceOnConveyorBelt()
    {
        auto input = GenerateMemory();
        input->addTuple(false);
        input->addTuple(move(currentJob));
        currentJob = nullptr;
        return OperatePlugin(1, input);
    }

    unique_ptr<ITransaction> BeginProcess(shared_ptr<Memory> initialParameters) override
    {
        set<int> plugins;
        plugins.insert(0);
        return GenerateOperateTransaction(&AssemblyWorker::RetrieveJob, plugins, 0);
    }

    unique_ptr<ITransaction> Process(shared_ptr<TransactionResult> previousResult) override
    {
        queue<shared_ptr<Message>> msgs = CheckMessages();

        if(!msgs.empty())
        {
            stopRequested = true;
        }

        if(currentJob == nullptr)
        {
            if(stopRequested)
            {
                stopRequested = false;
                return nullptr;
            }

            if(previousResult->resultParameters == nullptr)
            {
                set<int> plugins;
                plugins.insert(0);
                return GenerateOperateTransaction(&AssemblyWorker::RetrieveJob, plugins, 0);
            }

            currentJob = move(get<0>(previousResult->resultParameters->getTuple<unique_ptr<Job>>(0)));
        }
        
        if(currentJob->operationTypes.empty())
        {
            set<int> plugins;
            plugins.insert(1);
            return GenerateOperateTransaction(&AssemblyWorker::PlaceOnConveyorBelt, plugins, 8);
        }

        pair<OperationType,int>& typePair = currentJob->operationTypes.front();        
        currentJob->operationTypes.pop();
        OperationType type = typePair.first;
        int subtype = typePair.second;
        set<int> plugins;

        switch (type)
        {
        case MANUAL:           

            switch (subtype)
            {
            case 1:
                return GenerateOperateTransaction(&AssemblyWorker::ManualOperation1, plugins, 1);

            case 2:
                return GenerateOperateTransaction(&AssemblyWorker::ManualOperation2, plugins, 2);

            case 3:
                return GenerateOperateTransaction(&AssemblyWorker::ManualOperation3, plugins, 3);

            case 4:
                return GenerateOperateTransaction(&AssemblyWorker::ManualOperation4, plugins, 4);

            case 5:
                return GenerateOperateTransaction(&AssemblyWorker::ManualOperation5, plugins, 5);

            default:
                return nullptr;
            }

        case DRILLING:
            plugins.insert(2);
            return GenerateOperateTransaction(&AssemblyWorker::OperateDrillPress, plugins, 6);

        case WELDING:
            plugins.insert(5);
            return GenerateOperateTransaction(&AssemblyWorker::OperateWeldingStation, plugins, 7);            
            
        default:
            return nullptr;
        }
    }

    void ClearMemory() override { }
};