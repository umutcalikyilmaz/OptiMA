#include <TransactionModels/CreateAgent.h>

namespace OptiMA
{
    CreateAgent::CreateAgent(Agent* owner, IAgentManager* manager, int targetType) : ProceduralTransaction(owner, CREATEAGENT, 0),
    targetType(targetType), begin(false), manager(manager), initialized(false) { }

    CreateAgent::CreateAgent(Agent* owner, IAgentManager* manager, int targetType, shared_ptr<Memory> initialParameters) : ProceduralTransaction(owner, CREATEAGENT, 1),
    targetType(targetType), begin(true), manager(manager), initialized(true), initialParameters(initialParameters) { }

    unique_ptr<TransactionResult> CreateAgent::Procedure()
    {
        try
        {
            try
            {
                if(begin)
                {
                    manager->CreateAgent(owner->GetAgentId(), owner->GetAgentType(), targetType, initialParameters);
                }
                else
                {
                    manager->CreateAgent(owner->GetAgentId(), owner->GetAgentType(), targetType);
                }
            }
            catch(UserAbortException e)
            {
                return make_unique<TransactionResult>(ABORTED, e.what());
            }
        }
        catch(exception e)
        {
            return make_unique<TransactionResult>(FAILED, e.what());
        }

        return make_unique<TransactionResult>(SUCCESSFUL);        
    }
}