#pragma once
#include <chrono>
#include <thread>
#include "OptiMA/AgentModels/AgentTemplate.h"
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorParameters.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"
#include "OptiMA/Benchmarks/Shared/RandomNumber.h"

using namespace OptiMA;

class AssemblyWorker final : public AgentTemplate<AssemblyWorker>
{
public:

    AssemblyWorker()
        : probabilityRnd_(0, 1, randomNumberSeed),
          currentJob_(nullptr)
    {
        for(int i = 0; i < 5; i++)
        {
            manualRandoms_[i] = std::make_unique<NormalRandom>(assemblyManualOperationMeans[i] * simulationTimeScale,
                assemblyManualOperationStds[i] * simulationTimeScale, randomNumberSeed * (i + 1));
        }
    }

    std::shared_ptr<Memory> selfStop()
    {
        return stopAgent(getAgentId());
    }

    std::shared_ptr<Memory> retrieveJob()
    {
        std::shared_ptr<Memory> operationResult = operatePlugin(0, nullptr);
        std::shared_ptr<Memory> res = std::make_shared<Memory>();

        if(operationResult == nullptr)
        {
            res->addTuple(false);
        }
        else
        {
            currentJob_ = std::move(std::get<0>(operationResult->getTuple<std::unique_ptr<Job>>(0)));

            auto operation = currentJob_->operationTypes.front();
            currentJob_->operationTypes.pop();

            std::set<int> requestedPlugins = getRequestedPlugins(operation);
            int subtype = getSubtype(operation);
            res->addTuple(true);
            res->addTuple(subtype, operation, requestedPlugins);
        }
        
        return res;
    }

    std::shared_ptr<Memory> operate(std::vector<std::pair<OperationType, int>> description)
    {
        for(std::pair<OperationType, int> p : description)
        {
            auto input = generateMemory();
            int duration;

            switch(p.first)
            {
            case MANUAL:
                duration = (int)manualRandoms_[p.second]->generate();
                std::this_thread::sleep_for(std::chrono::milliseconds(duration));
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

        std::shared_ptr<Memory> res = std::make_shared<Memory>();

        if(currentJob_->operationTypes.empty())
        {
            res->addTuple(false);
        }
        else
        {
            auto operation = currentJob_->operationTypes.front();
            currentJob_->operationTypes.pop();

            std::set<int> requestedPlugins = getRequestedPlugins(operation);
            int subtype = getSubtype(operation);
            res->addTuple(true);
            res->addTuple(subtype, operation, requestedPlugins);
            
        }
        
        return res;
    }

    std::shared_ptr<Memory> placeOnConveyorBelt()
    {
        auto input = generateMemory();
        input->addTuple(false);
        input->addTuple(move(currentJob_));
        operatePlugin(1, input);

        std::shared_ptr<Memory> res = std::make_shared<Memory>();

        auto msgs = checkMessages();
        res->addTuple(!msgs.empty());

        return res;
    }

    void clearMemory() override { }

private:

    std::unique_ptr<Job> currentJob_;
    std::unique_ptr<NormalRandom> manualRandoms_[5];
    UniformRandom probabilityRnd_;
    double timeScale_;

    std::set<int> getRequestedPlugins(std::vector<std::pair<OperationType, int>> description)
    {
        std::set<int> res;

        for(std::pair<OperationType, int> p : description)
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

    int getSubtype(std::vector<std::pair<OperationType, int>> description)
    {
        int digit = 1;
        int res = 0;

        for(std::pair<OperationType, int> p : description)
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
};