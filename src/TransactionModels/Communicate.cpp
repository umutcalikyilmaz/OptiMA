#include <TransactionModels/Communicate.h>

namespace OptiMA
{
    Communicate::Communicate(Agent* owner, Postmaster* postmaster, shared_ptr<Message> msg, int receiverId)
    : ProceduralTransaction(owner, COMMUNICATE, 0), postmaster(postmaster), msg(msg), receiverIdOrType(receiverId),
    toAll(false) { }

    Communicate::Communicate(Agent* owner, Postmaster* postmaster, shared_ptr<Message> msg, int receiverIdOrType,
    bool toAll) : ProceduralTransaction(owner, COMMUNICATE, 1), postmaster(postmaster), msg(msg),
    receiverIdOrType(receiverIdOrType), toAll(toAll) { }

    unique_ptr<TransactionResult> Communicate::Procedure()
    {
        try
        {
            try
            {
                if(toAll)
                {
                    postmaster->SendToType(owner->GetAgentId(), owner->GetAgentType(), receiverIdOrType, msg);
                }
                else
                {
                    postmaster->SendToId(owner->GetAgentId(), owner->GetAgentType(), receiverIdOrType, msg);
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

    Communicate::~Communicate() { }
}