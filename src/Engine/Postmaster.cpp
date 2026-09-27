#include "OptiMA/Engine/Postmaster.h"

namespace OptiMA
{
    bool Postmaster::checkSender(int receiverType, int senderType)
    {
        bool found = false;

        for(int com : communicators_[receiverType])
        {
            if(com == senderType)
            {
                found = true;
                break;
            }
        }

        return found;
    }

    void Postmaster::enterLog(long transactionId, int senderId, int senderType, int receiverId, int receiverType, std::shared_ptr<Message> msg,
    PostBox* box)
    {
        msg->senderId_ = senderId;
        msg->senderType_ = senderType;
        msg->receiverId_ = receiverId;
        msg->receiverType_ = receiverType;

        std::lock_guard<std::mutex> lock(logLock_);
        auto it = transactionLog_.find(transactionId);

        if(it == transactionLog_.end())
        {
            transactionLog_[transactionId] = std::vector<std::pair<std::shared_ptr<Message>, PostBox*>>();            
        }

        transactionLog_.at(transactionId).push_back(make_pair(msg, box));
    }

    void Postmaster::send(std::shared_ptr<Message> msg, PostBox* postbox)
    {
        msg->timeStamp_ = std::chrono::steady_clock::now().time_since_epoch().count() - startingTime_;
        postbox->sendMessage(typename PostBox::PostmasterKey {}, msg);
    }

    Postmaster::Postmaster(AgentManagerKey, std::map<int ,std::vector<int>>& agentIds, std::map<int,
        std::vector<int>>& communicators, int startingTime)
        : agentIds_(agentIds),
          communicators_(communicators),
          startingTime_(startingTime) { }

    void Postmaster::addAgent(AgentManagerKey, int agentId, int agentType, PostBox* postbox)
    {
        std::lock_guard<std::mutex> lock(postLock_);
        postBoxes_[agentId] = std::make_pair(postbox, agentType);
        agentIds_[agentType].push_back(agentId);
    }
    /*
    void Postmaster::removeAgent(int agentId, int agentType)
    {
        std::lock_guard<std::mutex> lock(postLock_);
        postBoxes_.erase(agentId);
        agentIds_[agentType].erase(remove(agentIds_[agentType].begin(), agentIds_[agentType].end(), agentId), agentIds_[agentType].end());        
    }
    */
    void Postmaster::sendToId(AgentKey, long transactionId, int senderId, int senderType, int receiverId,
        std::shared_ptr<Message> msg)
    {
        std::lock_guard<std::mutex> lock(postLock_);
        auto box = postBoxes_[receiverId];
        int receiverType = box.second;

        if(!checkSender(receiverType, senderType))
        {
            throw UnautorizedAccessException("The sender is not autorized to communicate with this type of agent");
        }

        enterLog(transactionId, senderId, senderType, receiverId, receiverType, msg, box.first);
    }

    void Postmaster::sendToType(AgentKey, long transactionId, int senderId, int senderType, int receiverType,
        std::shared_ptr<Message> msg)
    {
        if(!checkSender(receiverType, senderType))
        {
            throw UnautorizedAccessException("The sender is not autorized to communicate with this type of agent");
        }

        std::lock_guard<std::mutex> lock(postLock_);

        for(int receiverId : agentIds_[receiverType])
        {
            enterLog(transactionId, senderId, senderType, receiverId, receiverType, msg, postBoxes_[receiverId].first);
        }
    }

    void Postmaster::commit(TransactionKey, long transactionId)
    {
        std::lock_guard<std::mutex> lock(logLock_);
        
        auto it = transactionLog_.find(transactionId);

        if(it != transactionLog_.end())
        {
            for(auto p : it->second)
            {
                send(p.first, p.second);
            }
        }

        transactionLog_.erase(transactionId);
    }

    void Postmaster::rollback(TransactionKey, long transactionId)
    {
        transactionLog_.erase(transactionId);
    }

    Postmaster::~Postmaster(){ }
}