#pragma once
#include <chrono>
#include <thread>
#include <AgentModels/AgentTemplate.h>
#include <Benchmarks/FactoryFloor/FactoryFloorParameters.h>
#include <Benchmarks/FactoryFloor/Job.h>

using namespace OptiMA;

class FloorManager : public AgentTemplate<FloorManager>
{    
    queue<int> idleAssemblyWorkers;
    queue<int> idleTransporters;
    queue<int> idleInspectors;
    vector<shared_ptr<Job>> jobbbs;
    double lastActive;
    double timeStep;
    int totalJobNum;
    int completedJobNum;
    int conveyorBeltPopulation;
    int lastConveyorBeltPopulation;
    int inspectionQueuePopulation;
    int lastInspectionQueuePopulation;
    int maxAssemblyWorker;
    int assemblyWorkerPopulation;
    int maxTransporter;
    int transporterPopulation;
    int maxInspector;
    int inspectorPopulation;
    int targetId;
    int lastOperation;
    int dass = 0;
    int dt = 0;
    int dins = 0;
    bool conveyor;
    bool increase;
    bool initial;

public:

    FloorManager() : timeStep(managerTimeStep * simulationTimeScale) { }

    shared_ptr<Memory> InsertJobs(shared_ptr<vector<unique_ptr<Job>>> jobs)
    {
        shared_ptr<Memory> input = GenerateMemory();
        input->addTuple(jobs);
        OperatePlugin(0, input);
        return nullptr;
    }

    shared_ptr<Memory> GetCoveyorBeltCount()
    {
        shared_ptr<Memory> input = GenerateMemory();
        input->addTuple(true);
        return OperatePlugin(1, input);
    }

    shared_ptr<Memory> GetInspectionQueueCount()
    {
        conveyor = false;
        shared_ptr<Memory> input = GenerateMemory();
        input->addTuple(true);
        return OperatePlugin(3, input);
    }

    shared_ptr<Memory> DoNothing()
    {
        return nullptr;
    }

    unique_ptr<ITransaction> Decide()
    {
        set<int> plugins;

        if(conveyor)
        {
            plugins.insert(3);
            lastOperation = 9;
            return GenerateOperateTransaction(&FloorManager::GetInspectionQueueCount, plugins, 1);
        }
        else
        {
            plugins.insert(1);
            lastOperation = 14;
            return GenerateOperateTransaction(&FloorManager::DoNothing, plugins, 0);
        }
    }

    unique_ptr<ITransaction> EndCycle()
    {
        set<int> plugins;
        lastActive = chrono::steady_clock::now().time_since_epoch().count();
        lastOperation = 14;
        return GenerateOperateTransaction(&FloorManager::DoNothing, plugins, 2);
    }

    unique_ptr<ITransaction> BeginProcess(shared_ptr<Memory> initialParameters) override
    {
        maxAssemblyWorker = maximumAssemblyWorker;
        maxTransporter = maximumTransporter;
        maxInspector = maximumInspector;
        totalJobNum = totalJobNumber;
        completedJobNum = 0;
        conveyorBeltPopulation = 0;
        inspectionQueuePopulation = 0;
        
        set<int> plugins;
        plugins.insert(0);
        lastOperation = 14;
        lastActive = chrono::steady_clock::now().time_since_epoch().count();
        return GenerateOperateTransaction(&FloorManager::InsertJobs, plugins, 3, jobs);
    }

    unique_ptr<ITransaction> Process(shared_ptr<TransactionResult> previousResult) override
    {
        queue<shared_ptr<Message>> msgs = CheckMessages();

        while(!msgs.empty())
        {
            auto msg = msgs.front();
            msgs.pop();

            if(msg->GetSenderType() == 2)
            {
                if(msg->parameters != nullptr)
                {
                    if(++completedJobNum == totalJobNum)
                    {
                        return GenerateHaltProgramTransaction(nullptr);
                    }
                }
            }
        }

        set<int> plugins;
        double tempTimeStep;

        switch (lastOperation)
        {
        case 0:            
            lastConveyorBeltPopulation = get<0>(previousResult->resultParameters->getTuple<int>(0));
            increase = true;
            conveyor = true;

            if(lastConveyorBeltPopulation == 0 || lastConveyorBeltPopulation < conveyorBeltPopulation)
            {
                conveyorBeltPopulation = lastConveyorBeltPopulation;
                lastOperation = 1;                
                return GenerateGetAgentInfoTransaction(0, true);
            }
            else
            {
                conveyorBeltPopulation = lastConveyorBeltPopulation;
                lastOperation = 4;
                return GenerateGetAgentInfoTransaction(1, true);
            }

        case 1:        
            if(increase)
            {
                int c = previousResult->resultParameters->getTupleCount();

                for(int i = 0; i < c; i++)
                {
                    tuple<int, int, AgentStatus, long, long>& record = previousResult->resultParameters->
                    getTuple<int, int, AgentStatus, long, long>(i);

                    if(get<2>(record) == IDLE)
                    {
                        targetId = get<0>(record);
                        lastOperation = 2;                        
                        return GenerateStartAgentTransaction(targetId, nullptr);
                    }
                }

                if(c < maxAssemblyWorker)
                {
                    lastOperation = 3;
                    return GenerateCreateAgentTransaction(0, nullptr);
                }

                lastOperation = 4;
                increase = false;
                return GenerateGetAgentInfoTransaction(1, true);
            }
            else
            {
                int c = previousResult->resultParameters->getTupleCount();
                int count = 0;

                for(int i = 0; i < c; i++)
                {
                    tuple<int, int, AgentStatus, long, long>& record = previousResult->resultParameters->
                    getTuple<int, int, AgentStatus, long, long>(i);

                    if(get<2>(record) == ACTIVE)
                    {
                        targetId = get<0>(record);

                        if(++count > 1)
                        {
                            break;
                        }
                    }
                }

                if(count > 1)
                {
                    lastOperation = 8;
                    return GenerateCommunicateTransaction(GenerateMessage(), targetId);
                }                
                
                plugins.insert(3);
                lastOperation = 9;
                return GenerateOperateTransaction(&FloorManager::GetInspectionQueueCount, plugins, 1);
            }            

        case 2:
            plugins.insert(3);
            lastOperation = 9;
            return GenerateOperateTransaction(&FloorManager::GetInspectionQueueCount, plugins, 1);

        case 3:
            plugins.insert(3);
            lastOperation = 9;
            return GenerateOperateTransaction(&FloorManager::GetInspectionQueueCount, plugins, 1);

        case 4:
            if(increase)
            {
                int c = previousResult->resultParameters->getTupleCount();

                for(int i = 0; i < c; i++)
                {
                    tuple<int, int, AgentStatus, long, long>& record = previousResult->resultParameters->
                    getTuple<int, int, AgentStatus, long, long>(i);

                    if(get<2>(record) == IDLE)
                    {
                        targetId = get<0>(record);
                        lastOperation = 5;
                        return GenerateStartAgentTransaction(targetId, nullptr);
                    }
                }

                if(c < maxTransporter)
                {
                    lastOperation = 6;
                    return GenerateCreateAgentTransaction(1, nullptr);
                }
                else
                {
                    increase = false;
                    
                    if(conveyor)
                    {
                        lastOperation = 1;
                        return GenerateGetAgentInfoTransaction(0, true);
                    }
                    else
                    {
                        lastOperation = 10;
                        return GenerateGetAgentInfoTransaction(2, true);
                    }
                    
                }
            }
            else
            {
                int c = previousResult->resultParameters->getTupleCount();
                int count = 0;

                for(int i = 0; i < c; i++)
                {
                    tuple<int, int, AgentStatus, long, long>& record = previousResult->resultParameters->
                    getTuple<int, int, AgentStatus, long, long>(i);

                    if(get<2>(record) == ACTIVE)
                    {
                        targetId = get<0>(record);

                        if(++count > 1)
                        {
                            break;
                        }
                    }
                }

                if(count > 1)
                {
                    lastOperation = 7;
                    return GenerateCommunicateTransaction(GenerateMessage(), targetId);
                }                
                
                plugins.insert(3);
                lastOperation = 9;
                return GenerateOperateTransaction(&FloorManager::GetInspectionQueueCount, plugins, 1);
            }

        case 5:
            return Decide();

        case 6:
            return Decide();

        case 7:            
            return Decide();

        case 8:
            plugins.insert(3);
            lastOperation = 9;
            return GenerateOperateTransaction(&FloorManager::GetInspectionQueueCount, plugins, 1);

        case 9:
            lastInspectionQueuePopulation = get<0>(previousResult->resultParameters->getTuple<int>(0));
            increase = true;
            conveyor = false;

            if(lastInspectionQueuePopulation == 0 || lastInspectionQueuePopulation < inspectionQueuePopulation)
            {
                inspectionQueuePopulation = lastInspectionQueuePopulation;
                lastOperation = 4;
                return GenerateGetAgentInfoTransaction(1, true);
            }
            else
            {
                inspectionQueuePopulation = lastInspectionQueuePopulation;
                lastOperation = 10;
                return GenerateGetAgentInfoTransaction(2, true);
            }

        case 10:
            if(increase)
            {
                int c = 0;

                for(int i = 0; i < c; i++)
                {
                    tuple<int, int, AgentStatus, long, long>& record = previousResult->resultParameters->
                    getTuple<int, int, AgentStatus, long, long>(i);

                    if(get<2>(record) == IDLE)
                    {
                        targetId = get<0>(record);
                        lastOperation = 11;
                        return GenerateStartAgentTransaction(targetId, nullptr);
                    }
                }

                if(c < maxAssemblyWorker)
                {
                    lastOperation = 12;
                    return GenerateCreateAgentTransaction(2, nullptr);
                }
                
                increase = false;
                lastOperation = 4;
                return GenerateGetAgentInfoTransaction(1, true);
            }
            else
            {
                int c = previousResult->resultParameters->getTupleCount();
                int count = 0;

                for(int i = 0; i < c; i++)
                {
                    tuple<int, int, AgentStatus, long, long>& record = previousResult->resultParameters->
                    getTuple<int, int, AgentStatus, long, long>(i);

                    if(get<2>(record) == ACTIVE)
                    {
                        targetId = get<0>(record);

                        if(++count > 1)
                        {
                            break;
                        }
                    }
                }

                if(count > 1)
                {
                    lastOperation = 13;
                    return GenerateCommunicateTransaction(GenerateMessage(), targetId);
                }               
                
                return EndCycle();
            }

        case 11:
            return EndCycle();

        case 12:
            return EndCycle();

        case 13:            
            return EndCycle();

        case 14:
            tempTimeStep = initial ? 10 * timeStep : timeStep;
            initial = false;    
        
            if(chrono::steady_clock::now().time_since_epoch().count() - lastActive > tempTimeStep)
            {
                plugins.insert(1);
                lastOperation = 0;
                return GenerateOperateTransaction(&FloorManager::GetCoveyorBeltCount, plugins, 0);                
            }
            else
            {
                return GenerateOperateTransaction(&FloorManager::DoNothing, plugins, 2);
            }

        default:
            return nullptr;
        }
    }

    void ClearMemory() override { }
};