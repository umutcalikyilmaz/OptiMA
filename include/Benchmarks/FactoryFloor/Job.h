#pragma once
#include <Shared/Types.h>

enum OperationType
{
    MANUAL,
    DRILLING,
    WELDING
};

struct Job
{
    queue<pair<OperationType,int>> operationTypes;
    bool isSuccessful;
    int id;
};
