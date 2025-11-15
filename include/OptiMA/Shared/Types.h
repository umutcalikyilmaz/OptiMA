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
    enum class AgentOperationType : uint8_t
    {
        CREATE,
        CREATEANDSTART,
        DESTROY,
        START,
        STOP
    };

    enum class AgentStatus : uint8_t
    {
        ACTIVE,
        ASSIGNED,
        IDLE
    };

    enum class ParameterType : uint8_t
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

    enum class PluginStatus : uint8_t
    {
        FREE,
        SEIZED
    };

    enum class PluginType : uint8_t
    {
        SHAREABLE,
        NONSHAREABLE
    };

    enum class TransactionStatus : uint8_t
    {
        ABORTED,
        FAILED,
        SUCCESSFUL
    };

    enum class TransactionType : uint8_t
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