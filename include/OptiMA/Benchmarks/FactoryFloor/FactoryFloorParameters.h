#pragma once
#include "OptiMA/Shared/Types.h"
#include "OptiMA/Benchmarks/FactoryFloor/Job.h"

static shared_ptr<vector<unique_ptr<Job>>> jobs;
static condition_variable warmupCondition;
static condition_variable cooldownCondition;
const static double assemblyManualOperationMeans[5] {700, 1200, 1700, 2200, 3000};
const static double assemblyManualOperationStds[5] {65, 100, 120, 250, 250};
const static double drillingOperationMeans[2] = {1500, 2100};
const static double drillingOperationStds[2] = {200,250};
const static double weldingOperationMeans[2] = {1600, 3000};
const static double weldingOperationStds[2] = {250, 300};
const static double transpoterTraverseMean = 2000;
const static double transpoterTraverseStd = 170;
const static double inspectorReportMean = 800;
const static double inspectorReportStd = 75;
const static double conveyorBeltPickUpMean = 1200;
const static double conveyorBeltPickUpStd = 90;
const static double conveyorBeltPlaceMean = 1000;
const static double conveyorBeltPlaceStd = 100;
const static double inspectionQueuePickUpMean = 1000;
const static double inspectionQueuePickUpStd = 150;
const static double inspectionQueuePlaceMean = 1500;
const static double inspectionQueuePlaceStd = 120;
const static double qaScannerOperationMean = 500;
const static double qaScannerOperationStd = 100;
static double simulationTimeScale;
static double managerTimeStep = 100000000;
static int maximumAssemblyWorker;
static int maximumTransporter;
static int maximumInspector;
static int totalJobNumber;
static atomic_int completed = 0;
static atomic_int started = 0;
static unsigned randomNumberSeed;
static atomic_bool warmedUp = false;
static atomic_bool cooledDown = false;