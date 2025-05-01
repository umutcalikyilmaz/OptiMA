#include <TransactionModels/DestroyAgent.h>

namespace OptiMA
{
    DestroyAgent::DestroyAgent(Agent* owner, IAgentManager* manager, int targetId) : ProceduralTransaction(owner, DESTROYAGENT, 0),
    manager(manager), targetId(targetId) { }

    unique_ptr<TransactionResult> DestroyAgent::Procedure()
    {
        try
        {
            try
            {
                manager->DestroyAgent(owner->GetAgentId(), owner->GetAgentType(), targetId);
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