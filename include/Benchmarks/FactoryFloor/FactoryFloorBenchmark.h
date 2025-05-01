#pragma once
#include <Benchmarks/FactoryFloor/JobCreator.h>
#include <Benchmarks/FactoryFloor/AgentTemplates/AssemblyWorker.h>
#include <Benchmarks/FactoryFloor/AgentTemplates/FloorManager.h>
#include <Benchmarks/FactoryFloor/AgentTemplates/Inspector.h>
#include <Benchmarks/FactoryFloor/AgentTemplates/Transporter.h>
#include <Benchmarks/FactoryFloor/Plugins/AssemblyQueue.h>
#include <Benchmarks/FactoryFloor/Plugins/ConveyorBelt.h>
#include <Benchmarks/FactoryFloor/Plugins/DrillPress.h>
#include <Benchmarks/FactoryFloor/Plugins/InspectionQueue.h>
#include <Benchmarks/FactoryFloor/Plugins/QAScanner.h>
#include <Benchmarks/FactoryFloor/Plugins/WeldingStation.h>
#include <Engine/Driver.h>

using namespace OptiMA;

class FactoryFloorBenchmark
{
    Driver* drv;
    MultiAgentModel* mam;
    SchedulerSettings* schedulerSettings;
    vector<double> manualOperationCoefs;
    string jobFilePath;
    string estimatorFilePath;
    string statsFilePath;
    mutex timerLock;
    condition_variable wakingCondition;
    double manualOperationCoef;
    double drillingCoef;
    double weldingCoef;
    int minimumOperationNumber;
    int maximumOperationNumber;
    int initialAssemblyWorkerNumber;
    int initialTransporterNumber;
    int initialInspectorNumber;
    int threadNumber;
    int timeStep;
    int batchSize;
    atomic_bool completed;
    atomic_bool interrupted;
    bool useExisting;
    bool saveJobs;
    bool schedulerInitialized;
    bool estimatorFileSet;
    bool timeStepSet;
    bool batchSizeSet;
    bool threadNumSet;
    bool keepStats;
    

public:

    FactoryFloorBenchmark(): manualOperationCoef(1), drillingCoef(1), weldingCoef(1),
    manualOperationCoefs({1, 1, 1, 1, 1}), minimumOperationNumber(1), maximumOperationNumber(5), saveJobs(false),
    useExisting(false), schedulerInitialized(false), estimatorFileSet(false), threadNumSet(false),
    timeStepSet(false), batchSizeSet(false), keepStats(false)
    {
        simulationTimeScale = 1;
        totalJobNumber = 100;
        randomNumberSeed = 0;
    }

    void SetTimeScale(double timeScale)
    {
        simulationTimeScale = timeScale;
    }

    void SetJobNumber(int jobNumber)
    {
        totalJobNumber = jobNumber;
    }

    void SetOperationNumber(int minimumOperationNumber, int maximumOperationNumber)
    {
        this->minimumOperationNumber = minimumOperationNumber;
        this->maximumOperationNumber = maximumOperationNumber;
    }

    void SetOperationCoefficients(double manualOperationCoefficient, double drillingCoefficient, double weldingCoefficient)
    {
        this->manualOperationCoef = manualOperationCoefficient;
        this->drillingCoef = drillingCoefficient;
        this->weldingCoef = weldingCoefficient;
    }

    void SetManualOperationCoefficients(vector<double>& coefficients)
    {
        if(coefficients.size() != 5)
        {
            throw InvalidModelParameterException((char*)"Number of coefficients must be equal to 5");
        }
    
        for(int i = 0; i < 5; i++)
        {
            manualOperationCoefs[i] = coefficients[i];
        }
    }

    void SaveJobs(string filePath)
    {
        saveJobs = true;
        useExisting = false;
        jobFilePath = filePath;
    }

    void UseExistingJobs(string filePath)
    {
        saveJobs = false;
        useExisting = true;
        jobFilePath = filePath;
    }

    void SetEstimatorFile(string filePath)
    {
        estimatorFileSet = true;
        keepStats = false;
        estimatorFilePath = filePath;        
    }

    void SetSchedulerSettings(SchedulerSettings* schedulerSettings)
    {
        this->schedulerSettings = schedulerSettings;
        schedulerInitialized = true;
    }

    void SetAssemblyWorkerNumber(int initialNumber, int maximumNumber)
    {
        initialAssemblyWorkerNumber = initialNumber;
        maximumAssemblyWorker = maximumNumber;
    }

    void SetTransporterNumber(int initialNumber, int maximumNumber)
    {
        initialTransporterNumber = initialNumber;
        maximumTransporter = maximumNumber;
    }

    void SetInspectorNumber(int initialNumber, int maximumNumber)
    {
        initialInspectorNumber = initialNumber;
        maximumInspector = maximumNumber;
    }

    void SetThreadNumber(int threadNumber)
    {
        this->threadNumber = threadNumber;
        threadNumSet = true;
    }

    void SetTimeStep(double timeStep)
    {
        this->timeStep = timeStep;
        timeStepSet = true;
    }

    void SetBatchSize(int batchSize)
    {
        this->batchSize = batchSize;
        batchSizeSet = true;
    }

    void KeepStats(string filePath)
    {
        keepStats = true;
        estimatorFileSet = false;
        statsFilePath = filePath;        
    }

    void setSeed(unsigned seed)
    {
        randomNumberSeed = seed;
    }

    void keepTime(chrono::milliseconds duration)
    {
        unique_lock<mutex> lock(timerLock);
        interrupted = !wakingCondition.wait_for(lock, duration, [this]
        { 
            return completed.load(); 
        });

        if(interrupted)
        {
            drv->HaltProgram(nullptr);
        }        
    }

    double StartBenchmark()
    {
        JobCreator* jc;
        mam = new MultiAgentModel();
        drv = new Driver();
    
        if(useExisting)
        {
            jc = new JobCreator(jobFilePath);
        }
        else
        {
            if(saveJobs)
            {
                jc = new JobCreator(totalJobNumber, minimumOperationNumber, maximumOperationNumber, manualOperationCoef, 
                drillingCoef, weldingCoef, manualOperationCoefs, jobFilePath);
            }
            else
            {
                jc = new JobCreator(totalJobNumber, minimumOperationNumber, maximumOperationNumber, manualOperationCoef, 
                drillingCoef, weldingCoef, manualOperationCoefs);
            }
        }
    
        jobs = jc->CreateJobs();
        delete jc;
    
        if(threadNumSet)
        {
            mam->SetThreadNumber(threadNumber);
        }
    
        if(timeStepSet)
        {
            mam->SetTimeStep(timeStep);
        }
    
        if(batchSizeSet)
        {
            mam->SetBatchSize(batchSize);
        }
    
        if(schedulerInitialized)
        {
            if(schedulerSettings->optimized && !estimatorFileSet)
            {
                throw InvalidModelParameterException((char*)"Estimator file is needed for optimized scheduling");
            }
    
            mam->AddSchedulerSettings(schedulerSettings);
        }
    
        if(keepStats)
        {
            mam->KeepStatsFile(statsFilePath);
        }
    
        mam->UseDefaultEstimator(estimatorFilePath);
        mam->AddAgentTemplate<FloorManager>(3, 1, 1, true);
        mam->AddAgentTemplate<AssemblyWorker>(0, initialAssemblyWorkerNumber, maximumAssemblyWorker, true);
        mam->AddAgentTemplate<Transporter>(1, initialTransporterNumber, maximumTransporter, true);
        mam->AddAgentTemplate<Inspector>(2, initialInspectorNumber, maximumInspector, true);
        
    
        mam->AddPlugin<AssemblyQueue>(0, SHAREABLE);
        mam->AddPlugin<ConveyorBelt>(1, SHAREABLE);
        mam->AddPlugin<DrillPress>(2, NONSHAREABLE);
        mam->AddPlugin<InspectionQueue>(3, SHAREABLE);
        mam->AddPlugin<QAScanner>(4, NONSHAREABLE);
        mam->AddPlugin<WeldingStation>(5, NONSHAREABLE);
    
        mam->AddHaltingAgent(3);
        
        mam->AddSupervisor(3, 0);
        
        mam->AddSupervisor(3, 1);
        mam->AddSupervisor(3, 2);
        mam->AddCommunication(3, 0);
        mam->AddCommunication(3, 1);
        mam->AddCommunication(3, 2);
        
        mam->AllowPluginUse(0, 0);
        mam->AllowPluginUse(0, 1);
        mam->AllowPluginUse(0, 2);
        mam->AllowPluginUse(0, 5);
        mam->AllowPluginUse(1, 1);
        mam->AllowPluginUse(1, 3);
        
        mam->AllowPluginUse(2, 3);
        mam->AllowPluginUse(2, 4);
        
        mam->AllowPluginUse(3, 0);
        mam->AllowPluginUse(3, 1);
        mam->AllowPluginUse(3, 3);
        
        
        completed = false;
        interrupted = false;
        
        double beg = chrono::steady_clock::now().time_since_epoch().count();
        thread timerThread(&FactoryFloorBenchmark::keepTime, this, chrono::milliseconds((int)(15000 * simulationTimeScale * totalJobNumber)));        
        drv->StartModel(*mam);        
        double end = chrono::steady_clock::now().time_since_epoch().count();

        completed = true;
        wakingCondition.notify_one();
        timerThread.join();

        delete mam;
        delete drv;

        if(interrupted)
        {
            return -1;
        }
        else
        {
            return (end - beg) / 1000000000;
        }
        
    }
};