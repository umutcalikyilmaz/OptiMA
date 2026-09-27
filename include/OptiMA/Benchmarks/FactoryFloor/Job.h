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
    std::queue<std::vector<std::pair<OperationType,int>>> operationTypes;
    bool isSuccessful;
    int id;
};
