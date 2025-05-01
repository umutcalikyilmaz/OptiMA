#pragma once
#include <Shared/Message.h>
#include <TransactionModels/ProceduralTransaction.h>

namespace OptiMA
{
    class Communicate final : public ProceduralTransaction
    {
        Postmaster* postmaster;
        shared_ptr<Message> msg;
        int receiverIdOrType;
        bool toAll;

    public:

        Communicate(Agent* owner, Postmaster* postmaster, shared_ptr<Message> msg, int receiverId);

        Communicate(Agent* owner, Postmaster* postmaster, shared_ptr<Message> msg, int receiverIdOrType, bool toAll);

        unique_ptr<TransactionResult> Procedure() override;

        ~Communicate();
    };
}