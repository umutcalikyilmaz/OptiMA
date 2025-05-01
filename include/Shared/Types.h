#pragma once
#include <atomic>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

namespace OptiMA
{
    enum AgentStatus
    {
        ACTIVE,
        IDLE
    };

    enum ParameterType
    {
        AGENTSTATUS,
        BOOL,
        INT,
        LONG,
        DOUBLE,
        PLUGINSTATUS,
        SHAREDPOINTER,
        STRING        
    };

    enum PluginStatus
    {
        FREE,
        SEIZED
    };

    enum PluginType
    {
        SHAREABLE,
        NONSHAREABLE
    };

    enum TransactionStatus
    {
        ABORTED,
        FAILED,
        SUCCESSFUL
    };

    enum TransactionType
    {
        BEGIN,
        COMMUNICATE,
        CREATEAGENT,
        DESTROYAGENT,
        GETAGENTINFO,
        HALTPROGRAM,
        OPERATE,
        PROCESS,
        STARTAGENT,
        STOPAGENT
    };
}