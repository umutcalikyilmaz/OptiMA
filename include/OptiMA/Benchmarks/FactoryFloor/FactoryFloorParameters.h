#pragma once
#include "OptiMA/Shared/Types.h"

const static double assemblyManualOperationMeans[5] {700, 1200, 2100, 3000, 4300};
const static double assemblyManualOperationStds[5] {65, 100, 150, 250, 250};
const static double drillingOperationMeans[2] = {2200, 3000};
const static double drillingOperationStds[2] = {200,250};
const static double weldingOperationMeans[2] = {3200, 5000};
const static double weldingOperationStds[2] = {350, 500};
const static double transpoterTraverseMean = 2500;
const static double transpoterTraverseStd = 200;
const static double inspectorReportMean = 800;
const static double inspectorReportStd = 75;
const static double conveyorBeltPickUpMean = 1500;
const static double conveyorBeltPickUpStd = 150;
const static double conveyorBeltPlaceMean = 1000;
const static double conveyorBeltPlaceStd = 100;
const static double inspectionQueuePickUpMean = 500;
const static double inspectionQueuePickUpStd = 50;
const static double inspectionQueuePlaceMean = 2500;
const static double inspectionQueuePlaceStd = 200;
const static double qaScannerOperationMean = 4000;
const static double qaScannerOperationStd = 400;
static double simulationTimeScale;
static double managerTimeStep = 10000000000;
static int maximumAssemblyWorker;
static int maximumTransporter;
static int maximumInspector;
static int totalJobNumber;
static shared_ptr<vector<unique_ptr<Job>>> jobs;
static unsigned randomNumberSeed;