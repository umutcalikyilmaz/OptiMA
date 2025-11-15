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
        lock_guard<mutex> lock(logLock_);
        auto it = transactionLog_.find(transactionId);

        if(it == transactionLog_.end())
        {
            transactionLog_[transactionId] = vector<pair<AgentOperationType, int>>();            
        }

        transactionLog_.at(transactionId).push_back(make_pair(operation, parameter));
    }

    AgentManager::AgentManager(vector<IAgentFactory*>& agentFactories, vector<int>& agentTypes, vector<int>& initialNumbers,
    vector<int>& maxNumbers, vector<pair<int,int>>& relationships, vector<pair<int,int>>& communications, vector<pair<int,int>> pluginAccesses,
    vector<int>& initialAgents, PluginManager* pmanager, long startingTime) : agentCount_(0), initialAgents_(initialAgents),
    startingTime_(startingTime)
    {
        int c = 0;
        map<int,vector<int>> communicators;

        for(int type : agentTypes)
        {
            maxNumbers_[type] = maxNumbers[c];
            currentNumbers_[type] = initialNumbers[c];            
            supervisors_[type] = vector<int>();
            subordinates_[type] = vector<int>();
            communicators[type] = vector<int>();
            tools_[type] = vector<int>();
            agentIds_[type] = vector<int>();
            c++;
        }

        for(pair<int,int> p : relationships)
        {
            supervisors_[p.second].push_back(p.first);
            subordinates_[p.first].push_back(p.second);
        }

        for(pair<int,int> p : communications)
        {
            communicators[p.first].push_back(p.second);
            communicators[p.second].push_back(p.first);
        }

        for(pair<int,int> p : pluginAccesses)
        {
            tools_[p.first].push_back(p.second); 
        }

        c = 0;
        postmaster_ = new Postmaster(agentIds_, communicators, startingTime);

        for(int type : agentTypes)
        {
            agentPools_[type] = new AgentPool(agentFactories[c], type, supervisors_[type], subordinates_[type], communicators[type], tools_[type],
            this, pmanager, postmaster_);

            for(int i = 0; i < initialNumbers[c]; i++)
            {
                Agent* agent = agentPools_[type]->getAgent();
                agent->setAgentId(agentCount_);
                agentMap_[agentCount_] = make_pair(agent, type);
                postmaster_->addAgent(agentCount_, type, agentMap_[agentCount_].first->getPostBoxAddress());
                agentIds_[type].push_back(agentCount_);
                agentCount_++;
            }

            c++;
        }        

        for(pair<int,pair<Agent*,int>> p : agentMap_)
        {
            AgentInfo* ai = new AgentInfo();
            ai->agentId = p.first;
            ai->agentType = p.second.second;
            ai->status = AgentStatus::IDLE;
            ai->creationTime = chrono::steady_clock::now().time_since_epoch().count() - startingTime;
            ai->lastStatusChange = ai->creationTime;
            agentInfos_[p.first] = ai;
        }
    }

    Agent* AgentManager::seizeAgent(long transactionId, int agentType)
    {
        lock_guard<mutex> lock(agentLock_);

        for(int id : agentIds_.at(agentType))
        {
            if(agentInfos_.at(id)->status == AgentStatus::ACTIVE)
            {
                agentInfos_.at(id)->status = AgentStatus::ASSIGNED;
                Agent* res = agentMap_.at(id).first;
                res->setCurrentTransaction(transactionId);
                return res;
            }
        }        

        throw AgentUnavailableException("No available agent of the given type exists");
    }

    void AgentManager::releaseAgent(int agentId)
    {
        lock_guard<mutex> lock(agentLock_);
        agentInfos_.at(agentId)->status = AgentStatus::ACTIVE;
        agentMap_.at(agentId).first->setCurrentTransaction(-1);
    }

    void AgentManager::transferOwnership(int transactionId, int agentId)
    {
        lock_guard<mutex> lock(agentLock_);
        agentMap_.at(agentId).first->setCurrentTransaction(transactionId);
    }

    void AgentManager::requestCreateAgent(long transactionId, int senderId, int senderType, int targetType)
    {
        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to create this type of agent");
        }

        lock_guard<mutex> lock(agentLock_);

        if(currentNumbers_.at(targetType) == maxNumbers_.at(targetType))
        {
            throw AgentLimitException("Maximum number of agents for this agent type is exceeded.");
        }

        enterLog(transactionId, AgentOperationType::CREATE, targetType);
    }

    void AgentManager::requestCreateAndStartAgent(long transactionId, int senderId, int senderType, int targetType)
    {
        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to create this type of agent");
        }

        lock_guard<mutex> lock(agentLock_);

        if(currentNumbers_.at(targetType) == maxNumbers_.at(targetType))
        {
            throw AgentLimitException("Maximum number of agents for this agent type is exceeded.");
        }

        enterLog(transactionId, AgentOperationType::CREATEANDSTART, targetType);
    }

    void AgentManager::requestStartAgent(long transactionId, int senderId, int senderType, int targetId)
    {
        lock_guard<mutex> lock(agentLock_);
        int targetType = agentMap_.at(targetId).second;        

        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to start this type of agent");
        }

        enterLog(transactionId, AgentOperationType::START, targetId);
    }

    void AgentManager::requestStopAgent(long transactionId, int senderId, int senderType, int targetId)
    {
        lock_guard<mutex> lock(agentLock_);
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

    void AgentManager::requestDestroyAgent(long transactionId, int senderId, int senderType, int targetId)
    {
        lock_guard<mutex> lock(agentLock_);
        int targetType = agentMap_.at(targetId).second;

        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to destroy this type of agent");
        }

        if(agentMap_.at(targetId).first->getStatus() != AgentStatus::IDLE)
        {
            throw UnautorizedAccessException("The agent cannot be destroyed because it is not idle.");
        }

        enterLog(transactionId, AgentOperationType::DESTROY, targetId);
    }

    void AgentManager::createAgent(int targetType)
    {
        lock_guard<mutex> lock(agentLock_);

        currentNumbers_.at(targetType)++;

        AgentInfo* ai = new AgentInfo();
        ai->agentId = agentCount_;
        ai->agentType = targetType;
        ai->status = AgentStatus::IDLE;
        ai->creationTime = chrono::steady_clock::now().time_since_epoch().count() - startingTime_;
        ai->lastStatusChange = ai->creationTime;

        Agent* agent = agentPools_[targetType]->getAgent();
        agent->setAgentId(agentCount_);
        agentMap_[agentCount_] = make_pair(agent, targetType);
        agentInfos_[agentCount_] = ai;
        agentIds_.at(targetType).push_back(agentCount_);
        agentCount_++;
    }

    void AgentManager::createAndStartAgent(int targetType)
    {
        lock_guard<mutex> lock(agentLock_);
        currentNumbers_[targetType]++;

        AgentInfo* ai = new AgentInfo();
        ai->agentId = agentCount_;
        ai->agentType = targetType;
        ai->status = AgentStatus::IDLE;
        ai->creationTime = chrono::steady_clock::now().time_since_epoch().count() - startingTime_;
        ai->lastStatusChange = ai->creationTime;

        Agent* agent = agentPools_[targetType]->getAgent();
        agent->setAgentId(agentCount_);
        agentMap_[agentCount_] = make_pair(agent, targetType);
        agentInfos_[agentCount_] = ai;
        agentIds_.at(targetType).push_back(agentCount_);
        int targetId = agentCount_;
        agentCount_++;
    }

    void AgentManager::startAgent(int targetId)
    {
        lock_guard<mutex> lock(agentLock_);

        agentMap_.at(targetId).first->start();
        agentInfos_.at(targetId)->status = AgentStatus::ACTIVE;
        agentInfos_.at(targetId)->lastStatusChange = chrono::steady_clock::now().time_since_epoch().count() - startingTime_;
    }

    void AgentManager::stopAgent(int targetId)
    {
        lock_guard<mutex> lock(agentLock_);
        
        agentMap_.at(targetId).first->stop();
        agentInfos_.at(targetId)->status = AgentStatus::IDLE;
        agentInfos_.at(targetId)->lastStatusChange = chrono::steady_clock::now().time_since_epoch().count() - startingTime_;        
    }

    void AgentManager::destroyAgent(int targetId)
    {
        lock_guard<mutex> lock(agentLock_);
        int targetType = agentMap_.at(targetId).second;
        
        currentNumbers_.at(targetType)--;
        
        delete agentMap_.at(targetId).first;
        agentMap_.erase(targetId);

        delete agentInfos_.at(targetId);
        agentInfos_.erase(targetId);
        
        agentIds_.at(targetType).erase(std::remove(agentIds_.at(targetType).begin(), agentIds_.at(targetType).end(), targetId), agentIds_.at(targetType).end());
    }

    Postmaster* AgentManager::getPostmaster()
    {
        return postmaster_;
    }

    shared_ptr<Memory> AgentManager::getAgentInfo(int senderId, int senderType, int targetId)
    {
        lock_guard<mutex> lock(agentLock_);
        int targetType = agentMap_.at(targetId).second;

        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to access info of this type of agent");
        }

        AgentInfo* info = agentInfos_.at(targetId);
        auto res = make_shared<Memory>();
        res->addTuple(info->agentId, info->agentType, info->status, info->creationTime, info->lastStatusChange);
        return res;
    }

    shared_ptr<Memory> AgentManager::getAgentInfos(int senderId, int senderType, int targetType)
    {
        if(!checkSender(senderType, targetType))
        {
            throw UnautorizedAccessException("The sender is not autorized to access info of this type of agent");
        }

        auto res = make_shared<Memory>();
        int c = 0;

        lock_guard<mutex> lock(agentLock_);

        for(int targetId : agentIds_.at(targetType))
        {            
            AgentInfo* info = agentInfos_.at(targetId);
            res->addTuple(info->agentId, info->agentType, info->status, info->creationTime, info->lastStatusChange);            
        }

        return res;
    }

    void AgentManager::commit(long transactionId)
    {
        lock_guard<mutex> lock(logLock_);
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

    void AgentManager::rollback(long transactionId)
    {
        transactionLog_.erase(transactionId);
    }

    void AgentManager::startInitialAgents()
    {
        for(int type : initialAgents_)
        {
            for(int id : agentIds_.at(type))
            {
                agentMap_.at(id).first->start();
                agentInfos_.at(id)->status = AgentStatus::ACTIVE;
            }
        }
    }

    AgentManager::~AgentManager()
    {
        for(auto& [key, val] : agentPools_)
        {
            delete val;
        }

        for(auto& [key, val] : agentInfos_)
        {
            delete val;
        }

        delete postmaster_;
        
        maxNumbers_.clear();
        agentPools_.clear();
        agentMap_.clear();
        supervisors_.clear();
        subordinates_.clear();
        tools_.clear();        
    }
}