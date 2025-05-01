#include <TransactionModels/GetAgentInfo.h>

namespace OptiMA
{
    GetAgentInfo::GetAgentInfo(Agent* owner, IAgentManager* manager, int targetId) : ProceduralTransaction(owner, GETAGENTINFO, 0),
    forAll(false), manager(manager), targetIdOrType(targetId) { }

    GetAgentInfo::GetAgentInfo(Agent* owner, IAgentManager* manager, int targetIdOrType, bool forAll) : ProceduralTransaction(owner, GETAGENTINFO, 1),
    forAll(forAll), manager(manager), targetIdOrType(targetIdOrType) { }

    unique_ptr<TransactionResult> GetAgentInfo::Procedure()
    {
        shared_ptr<Memory> info;

        try
        {
            try
            {
                if(forAll)
                {
                    info = manager->GetAgentInfos(owner->GetAgentId(), owner->GetAgentType(), targetIdOrType);
                }
                else                
                {
                    info = manager->GetAgentInfo(owner->GetAgentId(), owner->GetAgentType(), targetIdOrType);
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
        
        return make_unique<TransactionResult>(SUCCESSFUL, info);
    }
}