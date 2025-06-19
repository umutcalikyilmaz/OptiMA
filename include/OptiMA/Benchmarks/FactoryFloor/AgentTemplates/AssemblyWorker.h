#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/AgentModels/AgentTemplate.h"
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"

using namespace OptiMA;

class AssemblyWorker : public AgentTemplate<AssemblyWorker>
{
private:

    unique_ptr<Job> currentJob_;
    NormalRandom* manualRandoms_[5];
    UniformRandom probabilityRnd_;
    double timeScale_;

    set<int> getRequestedPlugins(vector<pair<OperationType, int>> description)
    {
        set<int> res;

        for(pair<OperationType, int> p : description)
        {
            switch(p.first)
            {
            case DRILLING:
                res.insert(2);
                break;

            case WELDING:
                res.insert(5);
                break;
            }
        }

        return res;
    }

    int getSubtype(vector<pair<OperationType, int>> description)
    {
        int digit = 1;
        int res = 0;

        for(pair<OperationType, int> p : description)
        {
            switch(p.first)
            {
            case MANUAL:
                res += (p.second) * digit; 
                break;

            case DRILLING:
                res += (p.second + 5) * digit;
                break;

            case WELDING:
                res += (p.second + 7) * digit;
                break;
            }

            digit *= 9;
        }

        return res;
    }

public:

    AssemblyWorker() : probabilityRnd_(0, 1, randomNumberSeed), currentJob_(nullptr)
    {
        for(int i = 0; i < 5; i++)
        {
            manualRandoms_[i] = new NormalRandom(assemblyManualOperationMeans[i] * simulationTimeScale, assemblyManualOperationStds[i]
            * simulationTimeScale, randomNumberSeed * (i + 1));
        }
    }

    shared_ptr<Memory> selfStop()
    {
        return stopAgent(getAgentId());
    }

    shared_ptr<Memory> retrieveJob()
    {
        shared_ptr<Memory> operationResult = operatePlugin(0, nullptr);
        shared_ptr<Memory> res = make_shared<Memory>();

        if(operationResult == nullptr)
        {
            res->addTuple(false);
        }
        else
        {
            currentJob_ = move(get<0>(operationResult->getTuple<unique_ptr<Job>>(0)));

            auto operation = currentJob_->operationTypes.front();
            currentJob_->operationTypes.pop();

            set<int> requestedPlugins = getRequestedPlugins(operation);
            int subtype = getSubtype(operation);
            res->addTuple(true);
            res->addTuple(subtype, operation, requestedPlugins);
        }
        
        return res;
    }

    shared_ptr<Memory> operate(vector<pair<OperationType, int>> description)
    {
        for(pair<OperationType, int> p : description)
        {
            auto input = generateMemory();
            int duration;

            switch(p.first)
            {
            case MANUAL:
                duration = (int)manualRandoms_[p.second]->generate();
                this_thread::sleep_for(chrono::milliseconds(duration));
                break;

            case DRILLING:
                input->addTuple(p.second);
                operatePlugin(2, input);
                break;

            case WELDING:
                input->addTuple(p.second);
                operatePlugin(5, input);
                break;
            }
        }

        shared_ptr<Memory> res = make_shared<Memory>();

        if(currentJob_->operationTypes.empty())
        {
            res->addTuple(false);
        }
        else
        {
            auto operation = currentJob_->operationTypes.front();
            currentJob_->operationTypes.pop();

            set<int> requestedPlugins = getRequestedPlugins(operation);
            int subtype = getSubtype(operation);
            res->addTuple(true);
            res->addTuple(subtype, operation, requestedPlugins);
            
        }
        
        return res;
    }

    shared_ptr<Memory> placeOnConveyorBelt()
    {
        auto input = generateMemory();
        input->addTuple(false);
        input->addTuple(move(currentJob_));
        operatePlugin(1, input);

        shared_ptr<Memory> res = make_shared<Memory>();

        auto msgs = checkMessages();
        res->addTuple(!msgs.empty());

        return res;
    }

    void clearMemory() override { }

    ~AssemblyWorker()
    {
        for(int i = 0; i < 5; i++)
        {
            delete manualRandoms_[i];
        }
    }
};