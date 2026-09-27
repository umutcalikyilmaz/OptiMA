#pragma once
#include "OptiMA/Benchmarks/FactoryFloor/JobCreator.h"
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/AssemblyWorker.h"
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/FloorManager.h"
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Inspector.h"
#include "OptiMA/Benchmarks/FactoryFloor/AgentTemplates/Transporter.h"
#include "OptiMA/Benchmarks/FactoryFloor/Plugins/AssemblyQueue.h"
#include "OptiMA/Benchmarks/FactoryFloor/Plugins/ConveyorBelt.h"
#include "OptiMA/Benchmarks/FactoryFloor/Plugins/DrillPress.h"
#include "OptiMA/Benchmarks/FactoryFloor/Plugins/InspectionQueue.h"
#include "OptiMA/Benchmarks/FactoryFloor/Plugins/OutputBin.h"
#include "OptiMA/Benchmarks/FactoryFloor/FactoryFloorTransactionFactory.h"
#include "OptiMA/Benchmarks/FactoryFloor/Plugins/QAScanner.h"
#include "OptiMA/Benchmarks/FactoryFloor/Plugins/WeldingStation.h"
#include "OptiMA/Engine/Driver.h"

using namespace OptiMA;

class FactoryFloorBenchmark
{
public:

    FactoryFloorBenchmark()
        : manualOperationCoef_(1),
          drillingCoef_(1),
          weldingCoef_(1),
          manualOperationCoefs_({1, 1, 1, 1, 1}),
          drillingOperationCoefs_({1, 1}),
          weldingOperationCoefs_({1, 1}),
          minimumTransactionNumber_(1),
          maximumTransactionNumber_(4),
          minimumOperationNumber_(1),
          maximumOperationNumber_(2),
          saveJobs_(false),
          useExisting_(false),
          schedulerInitialized_(false),
          estimatorFileSet_(false),
          threadNumSet_(false),
          batchSizeSet_(false),
          timeoutSet_(false),
          keepStats_(false)
    {
        simulationTimeScale = 1;
        totalJobNumber = 100;
        randomNumberSeed = 0;
    }

    void setTimeScale(double timeScale)
    {
        simulationTimeScale = timeScale;
    }

    void setJobNumber(int jobNumber)
    {
        totalJobNumber = jobNumber;
    }

    void setTransactionNumber(int minimumTransactionNumber, int maximumTransactionNumber)
    {
        minimumTransactionNumber_ = minimumTransactionNumber;
        maximumTransactionNumber_ = maximumTransactionNumber;
    }

    void setOperationNumber(int minimumOperationNumber, int maximumOperationNumber)
    {
        minimumOperationNumber_ = minimumOperationNumber;
        maximumOperationNumber_ = maximumOperationNumber;
    }

    void setOperationCoefficients(double manualOperationCoefficient, double drillingCoefficient, double weldingCoefficient)
    {
        manualOperationCoef_ = manualOperationCoefficient;
        drillingCoef_ = drillingCoefficient;
        weldingCoef_ = weldingCoefficient;
    }

    void setManualOperationCoefficients(double coefficient1, double coefficient2, double coefficient3, double coefficient4, double coefficient5)
    {
        manualOperationCoefs_[0] = coefficient1;
        manualOperationCoefs_[1] = coefficient2;
        manualOperationCoefs_[2] = coefficient3;
        manualOperationCoefs_[3] = coefficient4;
        manualOperationCoefs_[4] = coefficient5;
    }

    void saveJobs(std::string filePath)
    {
        saveJobs_ = true;
        useExisting_ = false;
        jobFilePath_ = filePath;
    }

    void useExistingJobs(std::string filePath)
    {
        saveJobs_ = false;
        useExisting_ = true;
        jobFilePath_ = filePath;
    }

    void setEstimatorFile(std::string filePath)
    {
        estimatorFileSet_ = true;
        keepStats_ = false;
        estimatorFilePath_ = filePath;        
    }

    void setSchedulerSettings(SchedulerSettings* schedulerSettings)
    {
        schedulerSettings_ = schedulerSettings;
        schedulerInitialized_ = true;
    }

    void setAssemblyWorkerNumber(int initialNumber, int maximumNumber)
    {
        initialAssemblyWorkerNumber_ = initialNumber;
        maximumAssemblyWorker = maximumNumber;
    }

    void setTransporterNumber(int initialNumber, int maximumNumber)
    {
        initialTransporterNumber_ = initialNumber;
        maximumTransporter = maximumNumber;
    }

    void setInspectorNumber(int initialNumber, int maximumNumber)
    {
        initialInspectorNumber_ = initialNumber;
        maximumInspector = maximumNumber;
    }

    void setThreadNumber(int threadNumber)
    {
        threadNumber_ = threadNumber;
        threadNumSet_ = true;
    }

    void setBatchSize(int batchSize)
    {
        batchSize_ = batchSize;
        batchSizeSet_ = true;
    }

    void setTimeout(std::chrono::milliseconds timeout)
    {
        timeout_ = timeout;
        timeoutSet_ = true;
    }

    void keepStats(std::string filePath)
    {
        keepStats_ = true;
        estimatorFileSet_ = false;
        statsFilePath_ = filePath;        
    }

    void setSeed(unsigned seed)
    {
        randomNumberSeed = seed;
    }

    void keepTime(std::chrono::milliseconds duration)
    {
        {
            std::unique_lock<std::mutex> lock(timerLock_);
            warmupCondition.wait(lock, [this]
            {
                return warmedUp.load();
            });
    
            beg_ = std::chrono::steady_clock::now().time_since_epoch().count();
            begCount_ = started;
            begCount2_ = completed;
        }
        

        std::unique_lock<std::mutex> lock(timerLock_);
        cooldownCondition.wait(lock, [this]
        {
            return cooledDown.load();
        });

        end_ = std::chrono::steady_clock::now().time_since_epoch().count();
        endCount_ = started;
        endCount2_ = completed;
        drv_->haltProgram(nullptr);
    }

    double StartBenchmark()
    {
        std::unique_ptr<JobCreator> jc;
        MultiAgentModel mam;
        drv_ = std::make_unique<Driver>();
        warmedUp = false;
        cooledDown = false;
        begCount_ = 0;
        endCount_ = 0;
        completed = 0;
        started = 0;
    
        if(useExisting_)
        {
            jc = std::make_unique<JobCreator>(jobFilePath_);
        }
        else
        {
            if(saveJobs_)
            {
                jc = std::make_unique<JobCreator>(totalJobNumber, minimumTransactionNumber_,
                    maximumTransactionNumber_,  minimumOperationNumber_, maximumOperationNumber_, 
                    manualOperationCoef_, drillingCoef_, weldingCoef_, manualOperationCoefs_,
                    drillingOperationCoefs_, weldingOperationCoefs_, jobFilePath_);
            }
            else
            {
                jc = std::make_unique<JobCreator>(totalJobNumber, minimumTransactionNumber_,
                    maximumTransactionNumber_, minimumOperationNumber_, maximumOperationNumber_,
                    manualOperationCoef_, drillingCoef_, weldingCoef_, manualOperationCoefs_,
                    drillingOperationCoefs_, weldingOperationCoefs_);
            }
        }
    
        jobs = jc->createJobs();
    
        if(threadNumSet_)
        {
            mam.setThreadNumber(threadNumber_);
        }
        else
        {
            throw InvalidModelParameterException((char*)"Thread number is not set");
        }
    
        if(batchSizeSet_)
        {
            mam.setBatchSize(batchSize_);
        }        
    
        if(schedulerInitialized_)
        {
            if(schedulerSettings_->optimized && !estimatorFileSet_)
            {
                throw InvalidModelParameterException((char*)"Estimator file is needed for optimized scheduling");
            }

            if(schedulerSettings_->optimized && (!timeoutSet_ || !batchSizeSet_))
            {
                throw InvalidModelParameterException((char*)"Batch size and timeout must be set for optimized execution");
            }
    
            mam.setSchedulerSettings(schedulerSettings_);
        }
        
        if(schedulerSettings_->optimized)
        {
            mam.setBatchSize(batchSize_);
            mam.setTimeout(timeout_);
        }
    
        if(keepStats_)
        {
            mam.keepStatsFile(statsFilePath_);
        }
    
        mam.useDefaultEstimator(estimatorFilePath_);
        mam.addAgentTemplate<FloorManager>(3, 1, 1, true);
        mam.addAgentTemplate<AssemblyWorker>(0, initialAssemblyWorkerNumber_, maximumAssemblyWorker, true);
        mam.addAgentTemplate<Transporter>(1, initialTransporterNumber_, maximumTransporter, true);
        mam.addAgentTemplate<Inspector>(2, initialInspectorNumber_, maximumInspector, true);        
    
        mam.addPlugin<AssemblyQueue>(0, PluginType::SHAREABLE);
        mam.addPlugin<ConveyorBelt>(1, PluginType::SHAREABLE);
        mam.addPlugin<DrillPress>(2, PluginType::NONSHAREABLE);
        mam.addPlugin<InspectionQueue>(3, PluginType::SHAREABLE);
        mam.addPlugin<QAScanner>(4, PluginType::NONSHAREABLE);
        mam.addPlugin<WeldingStation>(5, PluginType::NONSHAREABLE);
        mam.addPlugin<OutputBin>(6, PluginType::SHAREABLE);
           
        mam.addSupervisor(3, 0);        
        mam.addSupervisor(3, 1);
        mam.addSupervisor(3, 2);
        mam.addCommunication(3, 0);
        mam.addCommunication(3, 1);
        mam.addCommunication(3, 2);
        
        mam.allowPluginUse(0, 0);
        mam.allowPluginUse(0, 1);
        mam.allowPluginUse(0, 2);
        mam.allowPluginUse(0, 5);
        
        mam.allowPluginUse(1, 1);
        mam.allowPluginUse(1, 3);
        
        mam.allowPluginUse(2, 3);
        mam.allowPluginUse(2, 4);
        mam.allowPluginUse(2, 6);
        
        mam.allowPluginUse(3, 0);
        mam.allowPluginUse(3, 1);
        mam.allowPluginUse(3, 3);
        
        FactoryFloorTransactionFactory tf(initialAssemblyWorkerNumber_, initialTransporterNumber_, initialInspectorNumber_);
        mam.setTransactionFactory(&tf);
        
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();
        std::thread timerThread(&FactoryFloorBenchmark::keepTime, this, std::chrono::milliseconds((int)(15000 * simulationTimeScale * totalJobNumber)));
        drv_->startModel(mam);
        double end = std::chrono::steady_clock::now().time_since_epoch().count();

        wakingCondition_.notify_one();
        timerThread.join();

        return (double)(endCount2_ - begCount_) / ((end_ - beg_) / 1000000000);      
    }

private:

    std::unique_ptr<Driver> drv_;
    SchedulerSettings* schedulerSettings_;
    std::vector<double> manualOperationCoefs_;
    std::vector<double> drillingOperationCoefs_;
    std::vector<double> weldingOperationCoefs_;
    std::string jobFilePath_;
    std::string estimatorFilePath_;
    std::string statsFilePath_;
    std::chrono::milliseconds timeout_;
    std::mutex timerLock_;
    std::condition_variable wakingCondition_;
    double manualOperationCoef_;
    double drillingCoef_;
    double weldingCoef_;
    double beg_;
    double end_;
    int minimumTransactionNumber_;
    int maximumTransactionNumber_;
    int minimumOperationNumber_;
    int maximumOperationNumber_;
    int initialAssemblyWorkerNumber_;
    int initialTransporterNumber_;
    int initialInspectorNumber_;
    int threadNumber_;
    int timeStep_;
    int batchSize_;
    int begCount_;
    int begCount2_;
    int endCount_;
    int endCount2_;
    bool useExisting_;
    bool saveJobs_;
    bool schedulerInitialized_;
    bool estimatorFileSet_;
    bool batchSizeSet_;
    bool timeoutSet_;
    bool threadNumSet_;
    bool keepStats_;  
};