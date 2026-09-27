#include "OptiMA/Engine/AgentManager.h"

namespace OptiMA
{
    bool AgentManager::checkSender(int senderType, int targetType)
    {
        bool found = false;

        for(int sup : supervisors_[targetType])
        {
            if(sup == senderType)
            {
                found = true;
                break;
            }
        }

        return found;
    }

    void AgentManager::enterLog(long transactionId, AgentOperationType operation, int parameter)
    {
        std::lock_guard<std::mutex> lock(logLock_);
        auto it = transactionLog_.find(transactionId);

        if(it == transactionLog_.end())
        {
            transactionLog_[transactionId] = std::vector<std::pair<AgentOperationType, int>>();            
        }

        transactionLog_.at(transactionId).push_back(std::make_pair(operation, parameter));
    }

    AgentManager::AgentManager(DriverKey, const std::vector<std::unique_ptr<IAgentFactory>>& agentFactories,
        const std::vector<int>& agentTypes, const std::vector<int>& initialNumbers,
        const std::vector<int>& maxNumbers, const std::vector<std::pair<int,int>>& relationships,
        const std::vector<std::pair<int,int>>& communications,
        const std::vector<std::pair<int,int>>& pluginAccesses,
        const std::vector<int>& initialAgents, PluginManager* pmanager, long startingTime)
        : agentCount_(0),
          initialAgents_(initialAgents),
          startingTime_(startingTime)
    {
        int c = 0;
        std::map<int, std::vector<int>> communicators;

        for(int type : agentTypes)
        {
            maxNumbers_[type] = maxNumbers[c];
            currentNumbers_[type] = initialNumbers[c];            
            supervisors_[type] = std::vector<int>();
            subordinates_[type] = std::vector<int>();
            communicators[type] = std::vector<int>();
            tools_[type] = std::vector<int>();
            agentIds_[type] = std::vector<int>();
            c++;
        }

        for(std::pair<int,int> p : relationships)
        {
            supervisors_[p.second].push_back(p.first);
            subordinates_[p.first].push_back(p.second);
        }

        for(std::pair<int,int> p : communications)
        {
            communicators[p.first].push_back(p.second);
            communicators[p.second].push_back(p.first);
        }

        for(std::pair<int,int> p : pluginAccesses)
        {
            tools_[p.first].push_back(p.second); 
        }

        c = 0;
        postmaster_ = std::make_unique<Postmaster>(typename Postmaster::AgentManagerKey {}, agentIds_,
            communicators, startingTime);

        for(int type : agentTypes)
        {
            agentPools_[type] = std::make_unique<AgentPool>(typename AgentPool::AgentManagerKey {},
                agentFactories[c].get(), type, supervisors_[type], subordinates_[type], communicators[type],
                tools_[type], this, pmanager, postmaster_.get());

            for(int i = 0; i < initialNumbers[c]; i++)
            {
                std::unique_ptr<Agent> agent = agentPools_[type]->getAgent(typename AgentPool::AgentManagerKey {});
                agent->setAgentId(typename Agent::ManagerKey {}, agentCount_);
                agentMap_[agentCount_] = std::make_pair(std::move(agent), type);
                postmaster_->addAgent(typename Postmaster::AgentManagerKey {}, agentCount_, type,
                    agentMap_[agentCount_].first->getPostBoxAddress(typename Agent::ManagerKey {}));
                agentIds_[type].push_back(agentCount_);
                agentCount_++;
            }

            c++;
        }        

        for(auto& p : agentMap_)
        {
            auto ai = std::make_unique<AgentInfo>();
            ai->agentId = p.first;
            ai->agentType = p.second.second;
            ai->status = AgentStatus::IDLE;
            ai->creationTime = std::chrono::steady_clock::now().time_since_epoch().count() - startingTime;
            ai->lastStatusChange = ai->creationTime;
            agentInfos_[p.first] = std::move(ai);
        }
    }

    Agent* AgentManager::seizeAgent(TransactionKey, long transactionId, int agentType)
    {
        std::lock_guard<std::mutex> lock(agentLock_);

        for(int id : agentIds_.at(agentType))
        {
            if(agentInfos_.at(id)->status == AgentStatus::ACTIVE)
            {
                agentInfos_.at(id)->status = AgentStatus::ASSIGNED;
                auto res = agentMap_.at(id).first.get();
                res->setCurrentTransaction(typename Agent::ManagerKey {}, transactionId);
                return res;
            }
        }        

        throw AgentUnavailableException("No available agent of the given type exists");
    }

    void AgentManager::releaseAgent(TransactionKey, int agentId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        agentInfos_.at(agentId)->status = AgentStatus::ACTIVE;
        agentMap_.at(agentId).first->setCurrentTransaction(typename Agent::ManagerKey {}, -1);
    }

    void AgentManager::transferOwnership(TransactionKey, int transactionId, int agentId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        agentMap_.at(agentId).first->setCurrentTransaction(typename Agent::ManagerKey {}, transactionId);
    }

    void AgentManager::requestCreateAgent(AgentKey, long transactionId, int senderId, int senderType, int targetType)
    {
        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to create this type of agent");
        }

        std::lock_guard<std::mutex> lock(agentLock_);

        if(currentNumbers_.at(targetType) == maxNumbers_.at(targetType))
        {
            throw AgentLimitException("Maximum number of agents for this agent type is exceeded.");
        }

        enterLog(transactionId, AgentOperationType::CREATE, targetType);
    }

    void AgentManager::requestCreateAndStartAgent(AgentKey, long transactionId, int senderId, int senderType, int targetType)
    {
        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to create this type of agent");
        }

        std::lock_guard<std::mutex> lock(agentLock_);

        if(currentNumbers_.at(targetType) == maxNumbers_.at(targetType))
        {
            throw AgentLimitException("Maximum number of agents for this agent type is exceeded.");
        }

        enterLog(transactionId, AgentOperationType::CREATEANDSTART, targetType);
    }

    void AgentManager::requestStartAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        int targetType = agentMap_.at(targetId).second;        

        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to start this type of agent");
        }

        enterLog(transactionId, AgentOperationType::START, targetId);
    }

    void AgentManager::requestStopAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        int targetType = agentMap_[targetId].second;

        if(senderId != targetId)
        {
            if(!checkSender(senderType, targetType))
            {
                throw UnautorizedAccessException("The sender is not autorized to stop this type of agent");
            }

            if(agentInfos_[targetId]->status == AgentStatus::ASSIGNED)
            {
                throw UnautorizedAccessException("The agent cannot be stopped because it is assigned to a transaction");
            }
        }        

        enterLog(transactionId, AgentOperationType::START, targetId);
    }

    void AgentManager::requestDestroyAgent(AgentKey, long transactionId, int senderId, int senderType, int targetId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        int targetType = agentMap_.at(targetId).second;

        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to destroy this type of agent");
        }

        if(agentMap_.at(targetId).first->getStatus(typename Agent::ManagerKey {}) != AgentStatus::IDLE)
        {
            throw UnautorizedAccessException("The agent cannot be destroyed because it is not idle.");
        }

        enterLog(transactionId, AgentOperationType::DESTROY, targetId);
    }

    void AgentManager::createAgent(int targetType)
    {
        std::lock_guard<std::mutex> lock(agentLock_);

        currentNumbers_.at(targetType)++;

        auto ai = std::make_unique<AgentInfo>();
        ai->agentId = agentCount_;
        ai->agentType = targetType;
        ai->status = AgentStatus::IDLE;
        ai->creationTime = std::chrono::steady_clock::now().time_since_epoch().count() - startingTime_;
        ai->lastStatusChange = ai->creationTime;

        std::unique_ptr<Agent> agent = agentPools_[targetType]->getAgent(typename AgentPool::AgentManagerKey {});
        agent->setAgentId(typename Agent::ManagerKey {}, agentCount_);
        agentMap_[agentCount_] = std::make_pair(std::move(agent), targetType);
        agentInfos_[agentCount_] = move(ai);
        agentIds_.at(targetType).push_back(agentCount_);
        agentCount_++;
    }

    void AgentManager::createAndStartAgent(int targetType)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        currentNumbers_[targetType]++;

        auto ai = std::make_unique<AgentInfo>();
        ai->agentId = agentCount_;
        ai->agentType = targetType;
        ai->status = AgentStatus::IDLE;
        ai->creationTime = std::chrono::steady_clock::now().time_since_epoch().count() - startingTime_;
        ai->lastStatusChange = ai->creationTime;

        auto agent = agentPools_[targetType]->getAgent(typename AgentPool::AgentManagerKey {});
        agent->setAgentId(typename Agent::ManagerKey {}, agentCount_);
        agentMap_[agentCount_] = std::make_pair(std::move(agent), targetType);
        agentInfos_[agentCount_] = move(ai);
        agentIds_.at(targetType).push_back(agentCount_);
        int targetId = agentCount_;
        agentCount_++;
    }

    void AgentManager::startAgent(int targetId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);

        agentMap_.at(targetId).first->start(typename Agent::ManagerKey {});
        agentInfos_.at(targetId)->status = AgentStatus::ACTIVE;
        agentInfos_.at(targetId)->lastStatusChange = std::chrono::steady_clock::now().time_since_epoch().count() - startingTime_;
    }

    void AgentManager::stopAgent(int targetId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        
        agentMap_.at(targetId).first->stop(typename Agent::ManagerKey {});
        agentInfos_.at(targetId)->status = AgentStatus::IDLE;
        agentInfos_.at(targetId)->lastStatusChange = std::chrono::steady_clock::now().time_since_epoch().count() - startingTime_;        
    }

    void AgentManager::destroyAgent(int targetId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        int targetType = agentMap_.at(targetId).second;
        agentPools_.at(targetType)->returnAgent(typename AgentPool::AgentManagerKey {},
            std::move(agentMap_.at(targetId). first));
        
        currentNumbers_.at(targetType)--;        
        agentMap_.erase(targetId);
        agentInfos_.erase(targetId);        
        agentIds_.at(targetType).erase(std::remove(agentIds_.at(targetType).begin(), agentIds_.at(targetType).end(), targetId), agentIds_.at(targetType).end());
    }

    Postmaster* AgentManager::getPostmaster(DriverKey)
    {
        return postmaster_.get();
    }

    std::shared_ptr<Memory> AgentManager::getAgentInfo(AgentKey, int senderId, int senderType, int targetId)
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        int targetType = agentMap_.at(targetId).second;

        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to access info of this type of agent");
        }

        AgentInfo* info = agentInfos_.at(targetId).get();
        auto res = std::make_shared<Memory>();
        res->addTuple(info->agentId, info->agentType, info->status, info->creationTime, info->lastStatusChange);
        return res;
    }

    std::shared_ptr<Memory> AgentManager::getAgentInfos(AgentKey, int senderId, int senderType, int targetType)
    {
        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to access info of this type of agent");
        }

        auto res = std::make_shared<Memory>();
        int c = 0;

        std::lock_guard<std::mutex> lock(agentLock_);

        for(int targetId : agentIds_.at(targetType))
        {            
            AgentInfo* info = agentInfos_.at(targetId).get();
            res->addTuple(info->agentId, info->agentType, info->status, info->creationTime, info->lastStatusChange);            
        }

        return res;
    }

    void AgentManager::commit(TransactionKey, long transactionId)
    {
        std::lock_guard<std::mutex> lock(logLock_);
        auto it = transactionLog_.find(transactionId);

        if(it != transactionLog_.end())
        {            
            for(auto p : it->second)
            {
                switch (p.first)
                {
                case AgentOperationType::CREATE:
                    createAgent(p.second);
                    break;

                case AgentOperationType::CREATEANDSTART:
                    createAndStartAgent(p.second);
                    break;

                case AgentOperationType::DESTROY:
                    destroyAgent(p.second);
                    break;
                
                case AgentOperationType::START:
                    startAgent(p.second);
                    break;

                case AgentOperationType::STOP:
                    stopAgent(p.second);
                    break;
                default:
                    break;
                }
            }
        }
        
        transactionLog_.erase(transactionId);
    }

    void AgentManager::rollback(TransactionKey, long transactionId)
    {
        transactionLog_.erase(transactionId);
    }

    void AgentManager::startInitialAgents(DriverKey)
    {
        for(int type : initialAgents_)
        {
            for(int id : agentIds_.at(type))
            {
                agentMap_.at(id).first->start(typename Agent::ManagerKey {});
                agentInfos_.at(id)->status = AgentStatus::ACTIVE;
            }
        }
    }
}