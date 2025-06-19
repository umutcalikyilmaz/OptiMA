#pragma once
#include "OptiMA/Shared/Types.h"

namespace OptiMA
{
    struct AgentInfo
    {
        long creationTime;
        long lastStatusChange;
        int agentId;
        int agentType;
        AgentStatus status;
    };
    
}