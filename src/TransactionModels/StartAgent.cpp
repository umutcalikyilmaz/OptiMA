#include <TransactionModels/StartAgent.h>

namespace OptiMA
{
    StartAgent::StartAgent(Agent* owner, IAgentManager* manager, int targetId, shared_ptr<Memory> initialParameters)
    : ProceduralTransaction(owner, STARTAGENT, 0), manager(manager), targetId(targetId), initialParameters(initialParameters) { }

    unique_ptr<TransactionResult> StartAgent::Procedure()
    {
        try
        {
            try
            {
                manager->StartAgent(owner->GetAgentId(), owner->GetAgentType(), targetId, initialParameters);
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