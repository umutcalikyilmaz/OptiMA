#pragma once
#include "OptiMA/Shared/Types.h"

enum OperationType
{
    MANUAL,
    DRILLING,
    WELDING
};

struct Job
{
    queue<vector<pair<OperationType,int>>> operationTypes;
    bool isSuccessful;
    int id;
};
