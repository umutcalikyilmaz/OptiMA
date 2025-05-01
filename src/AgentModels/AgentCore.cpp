#include <AgentModels/AgentCore.h>

namespace OptiMA
{
    AgentCore::AgentCore() : started(false) { }

    void AgentCore::SetSupervisors(vector<int>& supervisors)
    {
        this->supervisors = supervisors;
    }

    void AgentCore::SetSubordinates(vector<int>& subordinates)
    {
        this->subordinates = subordinates;
    }

    void AgentCore::SetCommunications(vector<int>& contacts)
    {
        this->contacts = contacts;
    }

    void AgentCore::SetTools(vector<int>& tools)
    {
        this->allowedPlugins = tools;
    }

    void AgentCore::Start()
    {
        started = true;
    }

    void AgentCore::Stop()
    {
        started = false;
    }

    unique_ptr<ITransaction> AgentCore::RequestProcess(shared_ptr<TransactionResult> previousResult)
    {
        if(!started)
        {            
            return tfactory->GenerateNullTransaction();
        }

        return Process(previousResult);
    }

    unique_ptr<ITransaction> AgentCore::RequestBeginProcess(shared_ptr<Memory> initialParameters)
    {
        if(!started)
        {
            return tfactory->GenerateNullTransaction();
        }

        return BeginProcess(initialParameters);
    }
}