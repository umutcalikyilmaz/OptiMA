#pragma once
#include <AgentModels/AgentInfo.h>
#include <AgentModels/AgentTemplate.h>
#include <AgentModels/CorePool.h>
#include <Engine/PluginManager.h>

namespace OptiMA
{
    class Agent final
    {
        AgentCore* core;
        CorePool* corePool;
        int agentId;
        int agentType;
        AgentStatus status;

    public:

        Agent(AgentCore* core, CorePool* pool, int agentId, int agentType);

        unique_ptr<ITransaction> BeginProcess(shared_ptr<Memory> initialParameters);

        unique_ptr<ITransaction> Process(shared_ptr<TransactionResult> previousResult);

        void Start();

        void Stop();

        AgentStatus GetStatus();

        int GetAgentId();

        int GetAgentType();
        
        virtual ~Agent();

        friend class AgentManager;
    };
}