#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/AgentModels/AgentTemplate.h"
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"

using namespace OptiMA;

class FloorManager : public AgentTemplate<FloorManager>
{
    double lastActive_;
    double timeStep_;
    int totalJobNum_;
    int completedJobNum_;
    int conveyorBeltPopulation_;
    int lastConveyorBeltPopulation_;
    int inspectionQueuePopulation_;
    int lastInspectionQueuePopulation_;
    int maxAssemblyWorker_;
    int assemblyWorkerPopulation_;
    int maxTransporter_;
    int transporterPopulation_;
    int maxInspector_;
    int inspectorPopulation_;
    int targetId_;
    int lastOperation_;
    bool conveyor_;
    bool increase_;
    bool initial_;

    
public:

    FloorManager() : timeStep_(managerTimeStep * simulationTimeScale) { }

    shared_ptr<Memory> insertJobs()
    {
        initial_ = true;
        totalJobNum_ = totalJobNumber;
        completedJobNum_ = 0;
        shared_ptr<Memory> input = generateMemory();
        input->addTuple(jobs);
        operatePlugin(0, input);
        lastActive_ = chrono::steady_clock::now().time_since_epoch().count();
        return nullptr;
    }

    shared_ptr<Memory> manageConveyorBelt()
    {
        shared_ptr<Memory> input = generateMemory();
        input->addTuple(true);
        shared_ptr<Memory> operationResult = operatePlugin(1, input);
        lastConveyorBeltPopulation_ = get<0>(operationResult->getTuple<int>(0));

        if(lastConveyorBeltPopulation_ == 0 || lastConveyorBeltPopulation_ < conveyorBeltPopulation_)
        {
            conveyorBeltPopulation_ = lastConveyorBeltPopulation_;
            operationResult = getAgentInfoByType(0);
            int c = operationResult->getTupleCount();
            bool started = false;

            for(int i = 0; i < c; i++)
            {
                tuple<int, int, AgentStatus, long, long>& record = operationResult->getTuple<int, int, AgentStatus, long, long>(i);

                if(get<2>(record) == AgentStatus::IDLE)
                {
                    targetId_ = get<0>(record);
                    startAgent(targetId_);
                    started = true;
                    break;
                }
            }

            if(!started)
            {
                if(c < maxAssemblyWorker_)
                {
                    createAndStartAgent(0);
                }
                else
                {
                    operationResult = getAgentInfoByType(1);
                    c = operationResult->getTupleCount();
                    int count = 0;

                    for(int i = 0; i < c; i++)
                    {
                        tuple<int, int, AgentStatus, long, long>& record = operationResult->getTuple<int, int, AgentStatus, long, long>(i);

                        if(get<2>(record) == AgentStatus::ACTIVE || get<2>(record) == AgentStatus::ASSIGNED)
                        {
                            targetId_ = get<0>(record);

                            if(++count > 1)
                            {
                                break;
                            }
                        }
                    }

                    if(count > 1)
                    {
                        sendMessage(targetId_, generateMessage());
                    }                        
                }
            }  
        }
        else if(lastConveyorBeltPopulation_ > conveyorBeltPopulation_)
        {
            conveyorBeltPopulation_ = lastConveyorBeltPopulation_;
            operationResult = getAgentInfoByType(1);
            int c = operationResult->getTupleCount();
            bool started = false;

            for(int i = 0; i < c; i++)
            {
                tuple<int, int, AgentStatus, long, long>& record = operationResult->getTuple<int, int, AgentStatus, long, long>(i);

                if(get<2>(record) == AgentStatus::IDLE)
                {
                    targetId_ = get<0>(record);
                    startAgent(targetId_);
                    started = true;
                    break;
                }
            }

            if(!started)
            {
                if(c < maxTransporter_)
                {
                    createAndStartAgent(1);
                }
                else
                {
                    operationResult = getAgentInfoByType(0);
                    c = operationResult->getTupleCount();
                    int count = 0;

                    for(int i = 0; i < c; i++)
                    {
                        tuple<int, int, AgentStatus, long, long>& record = operationResult->getTuple<int, int, AgentStatus, long, long>(i);

                        if(get<2>(record) == AgentStatus::ACTIVE || get<2>(record) == AgentStatus::ASSIGNED)
                        {
                            targetId_ = get<0>(record);

                            if(++count > 1)
                            {
                                break;
                            }
                        }
                    }

                    if(count > 1)
                    {
                        sendMessage(targetId_, generateMessage());
                    }                        
                }
            } 
        }
        
        return nullptr;
    }

    shared_ptr<Memory> manageInspectionQueue()
    {
        shared_ptr<Memory> input = generateMemory();
        input->addTuple(true);
        shared_ptr<Memory> operationResult = operatePlugin(3, input);
        lastInspectionQueuePopulation_ = get<0>(operationResult->getTuple<int>(0));

        if(lastInspectionQueuePopulation_ == 0 || lastInspectionQueuePopulation_ < inspectionQueuePopulation_)
        {
            inspectionQueuePopulation_ = lastInspectionQueuePopulation_;
            operationResult = getAgentInfoByType(1);
            int c = operationResult->getTupleCount();
            bool started = false;

            for(int i = 0; i < c; i++)
            {
                tuple<int, int, AgentStatus, long, long>& record = operationResult->getTuple<int, int, AgentStatus, long, long>(i);

                if(get<2>(record) == AgentStatus::IDLE)
                {
                    targetId_ = get<0>(record);
                    startAgent(targetId_);
                    started = true;
                    break;
                }
            }

            if(!started)
            {
                if(c < maxTransporter_)
                {
                    createAndStartAgent(1);
                }
                else
                {
                    operationResult = getAgentInfoByType(2);
                    c = operationResult->getTupleCount();
                    int count = 0;

                    for(int i = 0; i < c; i++)
                    {
                        tuple<int, int, AgentStatus, long, long>& record = operationResult->getTuple<int, int, AgentStatus, long, long>(i);

                        if(get<2>(record) == AgentStatus::ACTIVE || get<2>(record) == AgentStatus::ASSIGNED)
                        {
                            targetId_ = get<0>(record);

                            if(++count > 1)
                            {
                                break;
                            }
                        }
                    }

                    if(count > 1)
                    {
                        sendMessage(targetId_, generateMessage());
                    }                        
                }
            }            
        }
        else if(lastInspectionQueuePopulation_ > inspectionQueuePopulation_)
        {
            inspectionQueuePopulation_ = lastInspectionQueuePopulation_;
            operationResult = getAgentInfoByType(2);
            int c = operationResult->getTupleCount();
            bool started = false;

            for(int i = 0; i < c; i++)
            {
                tuple<int, int, AgentStatus, long, long>& record = operationResult->getTuple<int, int, AgentStatus, long, long>(i);

                if(get<2>(record) == AgentStatus::IDLE)
                {
                    targetId_ = get<0>(record);
                    startAgent(targetId_);
                    started = true;
                    break;
                }
            }

            if(!started)
            {
                if(c < maxInspector_)
                {
                    createAndStartAgent(2);
                }
                else
                {
                    operationResult = getAgentInfoByType(1);
                    c = operationResult->getTupleCount();
                    int count = 0;

                    for(int i = 0; i < c; i++)
                    {
                        tuple<int, int, AgentStatus, long, long>& record = operationResult->getTuple<int, int, AgentStatus, long, long>(i);

                        if(get<2>(record) == AgentStatus::ACTIVE || get<2>(record) == AgentStatus::ASSIGNED)
                        {
                            targetId_ = get<0>(record);

                            if(++count > 1)
                            {
                                break;
                            }
                        }
                    }

                    if(count > 1)
                    {
                        sendMessage(targetId_, generateMessage());
                    }                        
                }
            }
        }

        lastActive_ = chrono::steady_clock::now().time_since_epoch().count();
        return nullptr;
    }

    shared_ptr<Memory> checkTime()
    {
        shared_ptr<Memory> res = generateMemory();

        double tempTimeStep = initial_ ? 10 * timeStep_ : timeStep_;
        initial_ = false;
        res->addTuple(chrono::steady_clock::now().time_since_epoch().count() - lastActive_ > tempTimeStep);
        return res;
    }

    void clearMemory() override { }
};