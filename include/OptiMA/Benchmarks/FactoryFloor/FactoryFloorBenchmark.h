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
private:

    Driver* drv_;
    MultiAgentModel* mam_;
    SchedulerSettings* schedulerSettings_;
    vector<double> manualOperationCoefs_;
    vector<double> drillingOperationCoefs_;
    vector<double> weldingOperationCoefs_;
    string jobFilePath_;
    string estimatorFilePath_;
    string statsFilePath_;
    mutex timerLock_;
    condition_variable wakingCondition_;
    double manualOperationCoef_;
    double drillingCoef_;
    double weldingCoef_;
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
    atomic_bool completed_;
    atomic_bool interrupted_;
    bool useExisting_;
    bool saveJobs_;
    bool schedulerInitialized_;
    bool estimatorFileSet_;
    bool trigger_;
    bool batchSizeSet_;
    bool threadNumSet_;
    bool keepStats_;
    

public:

    FactoryFloorBenchmark(): manualOperationCoef_(1), drillingCoef_(1), weldingCoef_(1), manualOperationCoefs_({1, 1, 1, 1, 1}),
    drillingOperationCoefs_({1, 1}), weldingOperationCoefs_({1, 1}), minimumTransactionNumber_(1), maximumTransactionNumber_(4),
    minimumOperationNumber_(1), maximumOperationNumber_(2), saveJobs_(false), useExisting_(false), schedulerInitialized_(false),
    estimatorFileSet_(false), threadNumSet_(false), trigger_(false), batchSizeSet_(false), keepStats_(false)
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

    void saveJobs(string filePath)
    {
        saveJobs_ = true;
        useExisting_ = false;
        jobFilePath_ = filePath;
    }

    void useExistingJobs(string filePath)
    {
        saveJobs_ = false;
        useExisting_ = true;
        jobFilePath_ = filePath;
    }

    void setEstimatorFile(string filePath)
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

    void setTrigger()
    {
        trigger_ = true;
    }

    void disableTrigger()
    {
        trigger_ = false;
    }

    void setBatchSize(int batchSize)
    {
        batchSize_ = batchSize;
        batchSizeSet_ = true;
    }

    void keepStats(string filePath)
    {
        keepStats_ = true;
        estimatorFileSet_ = false;
        statsFilePath_ = filePath;        
    }

    void setSeed(unsigned seed)
    {
        randomNumberSeed = seed;
    }

    void keepTime(chrono::milliseconds duration)
    {
        unique_lock<mutex> lock(timerLock_);
        interrupted_ = !wakingCondition_.wait_for(lock, duration, [this]
        { 
            return completed_.load(); 
        });

        if(interrupted_)
        {
            drv_->haltProgram(nullptr);
        }        
    }

    double StartBenchmark()
    {
        JobCreator* jc;
        mam_ = new MultiAgentModel();
        drv_ = new Driver();
    
        if(useExisting_)
        {
            jc = new JobCreator(jobFilePath_);
        }
        else
        {
            if(saveJobs_)
            {
                jc = new JobCreator(totalJobNumber, minimumTransactionNumber_, maximumTransactionNumber_,  minimumOperationNumber_,
                maximumOperationNumber_, manualOperationCoef_, drillingCoef_, weldingCoef_, manualOperationCoefs_, drillingOperationCoefs_,
                weldingOperationCoefs_, jobFilePath_);
            }
            else
            {
                jc = new JobCreator(totalJobNumber, minimumTransactionNumber_, maximumTransactionNumber_, minimumOperationNumber_,
                maximumOperationNumber_, manualOperationCoef_, drillingCoef_, weldingCoef_, manualOperationCoefs_, drillingOperationCoefs_,
                weldingOperationCoefs_);
            }
        }
    
        jobs = jc->createJobs();
        delete jc;
    
        if(threadNumSet_)
        {
            mam_->setThreadNumber(threadNumber_);
        }
        else
        {
            throw InvalidModelParameterException((char*)"Thread number is not set");
        }
    
        if(trigger_)
        {
            mam_->setTrigger();
        }
    
        if(batchSizeSet_)
        {
            mam_->setBatchSize(batchSize_);
        }
    
        if(schedulerInitialized_)
        {
            if(schedulerSettings_->optimized && !estimatorFileSet_)
            {
                throw InvalidModelParameterException((char*)"Estimator file is needed for optimized scheduling");
            }
    
            mam_->setSchedulerSettings(schedulerSettings_);
        }
    
        if(keepStats_)
        {
            mam_->keepStatsFile(statsFilePath_);
        }
    
        mam_->useDefaultEstimator(estimatorFilePath_);
        mam_->addAgentTemplate<FloorManager>(3, 1, 1, true);
        mam_->addAgentTemplate<AssemblyWorker>(0, initialAssemblyWorkerNumber_, maximumAssemblyWorker, true);
        mam_->addAgentTemplate<Transporter>(1, initialTransporterNumber_, maximumTransporter, true);
        mam_->addAgentTemplate<Inspector>(2, initialInspectorNumber_, maximumInspector, true);        
    
        mam_->addPlugin<AssemblyQueue>(0, PluginType::SHAREABLE);
        mam_->addPlugin<ConveyorBelt>(1, PluginType::SHAREABLE);
        mam_->addPlugin<DrillPress>(2, PluginType::NONSHAREABLE);
        mam_->addPlugin<InspectionQueue>(3, PluginType::SHAREABLE);
        mam_->addPlugin<QAScanner>(4, PluginType::NONSHAREABLE);
        mam_->addPlugin<WeldingStation>(5, PluginType::NONSHAREABLE);
        mam_->addPlugin<OutputBin>(6, PluginType::SHAREABLE);
           
        mam_->addSupervisor(3, 0);        
        mam_->addSupervisor(3, 1);
        mam_->addSupervisor(3, 2);
        mam_->addCommunication(3, 0);
        mam_->addCommunication(3, 1);
        mam_->addCommunication(3, 2);
        
        mam_->allowPluginUse(0, 0);
        mam_->allowPluginUse(0, 1);
        mam_->allowPluginUse(0, 2);
        mam_->allowPluginUse(0, 5);
        
        mam_->allowPluginUse(1, 1);
        mam_->allowPluginUse(1, 3);
        
        mam_->allowPluginUse(2, 3);
        mam_->allowPluginUse(2, 4);
        mam_->allowPluginUse(2, 6);
        
        mam_->allowPluginUse(3, 0);
        mam_->allowPluginUse(3, 1);
        mam_->allowPluginUse(3, 3);
        
        FactoryFloorTransactionFactory tf(initialAssemblyWorkerNumber_, initialTransporterNumber_, initialInspectorNumber_);
        mam_->setTransactionFactory(&tf);
        
        completed_ = false;
        interrupted_ = false;
        
        double beg = chrono::steady_clock::now().time_since_epoch().count();
        thread timerThread(&FactoryFloorBenchmark::keepTime, this, chrono::milliseconds((int)(15000 * simulationTimeScale * totalJobNumber)));        
        drv_->startModel(*mam_);        
        double end = chrono::steady_clock::now().time_since_epoch().count();

        completed_ = true;
        wakingCondition_.notify_one();
        timerThread.join();

        delete mam_;
        delete drv_;

        if(interrupted_)
        {
            return -1;
        }
        else
        {
            return (end - beg) / 1000000000;
        }
        
    }
};