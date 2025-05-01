#include <AgentModels/Agent.h>

namespace OptiMA
{
    

    Agent::Agent(AgentCore* core, CorePool* pool, int agentId, int agentType) : agentId(agentId),
    agentType(agentType), core(core), corePool(pool), status(IDLE)
    {
        this->core->SetOwner(this);
    }

    unique_ptr<ITransaction> Agent::Process(shared_ptr<TransactionResult> previousResult)
    {
        unique_ptr<ITransaction> txn = nullptr; 
        
        try
        {
            return core->RequestProcess(previousResult);            
        }
        catch(const std::exception& e) { }

        if(txn == nullptr)
        {
            status = IDLE;
            core->SelfStop(agentId, agentType);
        }

        return txn;
    }

    unique_ptr<ITransaction> Agent::BeginProcess(shared_ptr<Memory> initialParameters)
    {
        unique_ptr<ITransaction> txn = nullptr;

        try
        {
            txn = core->RequestBeginProcess(initialParameters);
        }
        catch(const std::exception& e) { }
        
        if(txn == nullptr)
        {
            status = IDLE;
            core->SelfStop(agentId, agentType);
        }

        return txn;
    }

    void Agent::Start()
    {
        status = ACTIVE;
        core->Start();
    }

    void Agent::Stop()
    {
        core->Stop();
    }

    AgentStatus Agent::GetStatus()
    {
        return status;
    }
        
    int Agent::GetAgentId()
    {
        return agentId;
    }

    int Agent::GetAgentType()
    {
        return agentType;
    }

    Agent::~Agent()
    {
        corePool->ReturnCore(core);
    }
}