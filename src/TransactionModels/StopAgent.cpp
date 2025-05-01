#include <TransactionModels/StopAgent.h>

namespace OptiMA
{
    StopAgent::StopAgent(Agent* owner, IAgentManager* manager, int targetId) : ProceduralTransaction(owner, STOPAGENT, 0), manager(manager),
    targetId(targetId) { }

    unique_ptr<TransactionResult> StopAgent::Procedure()
    {
        try
        {
            try
            {
                manager->StopAgent(owner->GetAgentId(), owner->GetAgentType(), targetId);
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