#include <Engine/AgentManager.h>

namespace OptiMA
{
    bool AgentManager::CheckSender(int senderType, int targetType)
    {
        bool found = false;

        for(int sup : supervisors[targetType])
        {
            if(sup == senderType)
            {
                found = true;
                break;
            }
        }

        return found;
    }

    void AgentManager::SendBeginTransaction(Agent* owner, shared_ptr<Memory> initialParameters)
    {
        listener->SendTransaction(make_unique<Begin>(owner, initialParameters));
    }

    AgentManager::AgentManager(vector<IAgentCoreFactory*>& coreFactories, vector<int>& coreIds,
    vector<int>& initialNumbers, vector<int>& maxNumbers, vector<pair<int,int>>& relationships,
    vector<pair<int,int>>& communications, vector<pair<int,int>> pluginAccesses, vector<int>& initialAgents,
    map<int,shared_ptr<Memory>>& initialParameters, PluginManager* pmanager, Listener* listener, long startingTime)
    : agentCount(0), initialAgents(initialAgents),initialParameters(initialParameters), listener(listener),
    startingTime(startingTime)
    {
        int c = 0;
        map<int,vector<int>> communicators;

        for(int type : coreIds)
        {
            this->maxNumbers[type] = maxNumbers[c];
            currentNumbers[type] = initialNumbers[c];            
            supervisors[type] = vector<int>();
            subordinates[type] = vector<int>();
            communicators[type] = vector<int>();
            tools[type] = vector<int>();
            agentIds[type] = vector<int>();
            c++;
        }

        for(pair<int,int> p : relationships)
        {
            supervisors[p.second].push_back(p.first);
            subordinates[p.first].push_back(p.second);
        }

        for(pair<int,int> p : communications)
        {
            communicators[p.first].push_back(p.second);
            communicators[p.second].push_back(p.first);
        }

        for(pair<int,int> p : pluginAccesses)
        {
            tools[p.first].push_back(p.second); 
        }

        c = 0;
        tfactory = new TransactionFactory();

        postmaster = new Postmaster(agentIds, communicators, startingTime);

        for(int type : coreIds)
        {
            corePools[type] = new CorePool(coreFactories[c], type, supervisors[type], subordinates[type],
            communicators[type], tools[type], this, tfactory, pmanager, postmaster);

            for(int i = 0; i < initialNumbers[c]; i++)
            {
                agentMap[agentCount] = make_pair(new Agent(corePools[type]->GetCore(), corePools[type], agentCount, type), type);                
                postmaster->AddAgent(agentCount, type, agentMap[agentCount].first->core->GetPostBoxAddress());
                agentIds[type].push_back(agentCount);
                agentCount++;
            }

            c++;
        }        

        for(pair<int,pair<Agent*,int>> p : agentMap)
        {
            AgentInfo* ai = new AgentInfo();
            ai->agentId = p.first;
            ai->agentType = p.second.second;
            ai->status = IDLE;
            ai->creationTime = chrono::steady_clock::now().time_since_epoch().count() - startingTime;
            ai->lastStatusChange = ai->creationTime;
            agentInfos[p.first] = ai;
        }
    }

    void AgentManager::CreateAgent(int senderId, int senderType, int targetType)
    {
        if(!CheckSender(senderType, targetType))
        {
            throw UnautorizedAccessException((char*)"The sender is not autorized to create this type of agent");
        }

        lock_guard<mutex> lock(agentLock);

        if(currentNumbers.at(targetType) == maxNumbers.at(targetType))
        {
            throw AgentLimitException((char*)"Maximum number of agents for this agent type is exceeded.");
        }

        currentNumbers.at(targetType)++;

        AgentInfo* ai = new AgentInfo();
        ai->agentId = agentCount;
        ai->agentType = targetType;
        ai->status = IDLE;
        ai->creationTime = chrono::steady_clock::now().time_since_epoch().count() - startingTime;
        ai->lastStatusChange = ai->creationTime;

        agentMap[agentCount] = make_pair(new Agent(corePools[targetType]->GetCore(), corePools[targetType], agentCount, targetType), targetType);
        agentInfos[agentCount] = ai;
        agentIds.at(targetType).push_back(agentCount);
        agentCount++;
    }

    void AgentManager::CreateAgent(int senderId, int senderType, int targetType, shared_ptr<Memory> initialParameters)
    {
        if(!CheckSender(senderType, targetType))
        {
            throw UnautorizedAccessException((char*)"The sender is not autorized to create this type of agent");
        }

        lock_guard<mutex> lock(agentLock);

        if(currentNumbers[targetType] == maxNumbers[targetType])
        {
            throw AgentLimitException((char*)"Maximum number of agents for this agent type is exceeded.");
        }

        currentNumbers[targetType]++;

        AgentInfo* ai = new AgentInfo();
        ai->agentId = agentCount;
        ai->agentType = targetType;
        ai->status = IDLE;
        ai->creationTime = chrono::steady_clock::now().time_since_epoch().count() - startingTime;
        ai->lastStatusChange = ai->creationTime;

        agentMap[agentCount] = make_pair(new Agent(corePools[targetType]->GetCore(), corePools[targetType], agentCount, targetType), targetType);
        agentInfos[agentCount] = ai;
        agentIds.at(targetType).push_back(agentCount);
        int targetId = agentCount;
        agentCount++;

        SendBeginTransaction(agentMap[targetId].first, initialParameters);
    }

    void AgentManager::StartAgent(int senderId, int senderType, int targetId, shared_ptr<Memory> initialParameters)
    {
        agentLock.lock();
        int targetType = agentMap.at(targetId).second;        

        if(!CheckSender(senderType, targetType))
        {
            agentLock.unlock();
            throw UnautorizedAccessException((char*)"The sender is not autorized to start this type of agent");
        }

        agentMap.at(targetId).first->Start();
        agentInfos.at(targetId)->status = ACTIVE;
        agentInfos.at(targetId)->lastStatusChange = chrono::steady_clock::now().time_since_epoch().count() - startingTime;
        agentLock.unlock();

        SendBeginTransaction(agentMap.at(targetId).first, initialParameters);
    }

    void AgentManager::StopAgent(int senderId, int senderType, int targetId)
    {
        lock_guard<mutex> lock(agentLock);
        int targetType = agentMap[targetId].second;

        if(senderId != targetId)
        {
            if(!CheckSender(senderType, targetType))
            {
                throw UnautorizedAccessException((char*)"The sender is not autorized to halt this type of agent");
            }
        }
        
        agentMap.at(targetId).first->Stop();
        agentInfos.at(targetId)->status = IDLE;
        agentInfos.at(targetId)->lastStatusChange = chrono::steady_clock::now().time_since_epoch().count() - startingTime;        
    }

    void AgentManager::DestroyAgent(int senderId, int senderType, int targetId)
    {
        lock_guard<mutex> lock(agentLock);
        int targetType = agentMap.at(targetId).second;

        if(!CheckSender(senderType, targetType))
        {
            throw UnautorizedAccessException((char*)"The sender is not autorized to destroy this type of agent");
        }

        if(agentMap.at(targetId).first->GetStatus() == ACTIVE)
        {
            throw UnautorizedAccessException((char*)"An active agent cannot be destroyed");
        }
        
        currentNumbers.at(targetType)--;
        
        delete agentMap.at(targetId).first;
        agentMap.erase(targetId);

        delete agentInfos.at(targetId);
        agentInfos.erase(targetId);
        
        agentIds.at(targetType).erase(std::remove(agentIds.at(targetType).begin(), agentIds.at(targetType).end(), targetId), agentIds.at(targetType).end());
    }

    shared_ptr<Memory>  AgentManager::GetAgentInfo(int senderId, int senderType, int targetId)
    {
        lock_guard<mutex> lock(agentLock);
        int targetType = agentMap.at(targetId).second;

        if(!CheckSender(senderType, targetType))
        {
            throw UnautorizedAccessException((char*)"The sender is not autorized to access info of this type of agent");
        }

        AgentInfo* info = agentInfos.at(targetId);
        auto res = make_shared<Memory>();
        res->addTuple(info->agentId, info->agentType, info->status, info->creationTime, info->lastStatusChange);
        return res;
    }

    shared_ptr<Memory> AgentManager::GetAgentInfos(int senderId, int senderType, int targetType)
    {
        if(!CheckSender(senderType, targetType))
        {
            throw UnautorizedAccessException((char*)"The sender is not autorized to access info of this type of agent");
        }

        auto res = make_shared<Memory>();
        int c = 0;

        lock_guard<mutex> lock(agentLock);

        for(int targetId : agentIds.at(targetType))
        {            
            AgentInfo* info = agentInfos.at(targetId);
            res->addTuple(info->agentId, info->agentType, info->status, info->creationTime, info->lastStatusChange);            
        }

        return res;
    }
        
    void AgentManager::StartInitialAgents()
    {
        for(int type : initialAgents)
        {
            for(int id : agentIds.at(type))
            {
                agentMap.at(id).first->Start();
                agentInfos.at(id)->status = ACTIVE;
                SendBeginTransaction(agentMap.at(id).first, initialParameters.at(type));
            }
        }
    }

    AgentManager::~AgentManager()
    {
        for(auto& [key, val] : agentMap)
        {
            delete val.first;
        }

        for(auto& [key, val] : corePools)
        {
            delete val;
        }

        delete postmaster;
        delete tfactory;
        
        maxNumbers.clear();
        corePools.clear();
        agentMap.clear();
        supervisors.clear();
        subordinates.clear();
        tools.clear();        
    }
}