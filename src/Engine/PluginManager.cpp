#include <Engine/PluginManager.h>

namespace OptiMA
{
    PluginManager::PluginManager(vector<IPluginInstanceFactory*>& factories, vector<PluginType> pluginTypes,
    vector<int> pluginIds, vector<pair<int,int>> pluginAccesses)
    {
        int c = 0;

        for(int id : pluginIds)
        {
            //instancePools[id] = new InstancePool(factories[c], pluginTypes[c], pluginIds[c]);
            instances[id] = factories[c]->CreatePluginInstance();
            instances[id]->pluginId = id;
            types[id] = pluginTypes[c];
            statuses[id] = FREE;
            allowedAgentTypes[id] = vector<int>();

            if(pluginTypes[c] == NONSHAREABLE)
            {
                nonShareable.insert(id);
            }

            c++;
        }

        for(pair<int,int> p : pluginAccesses)
        {
            allowedAgentTypes[p.second].push_back(p.first);
        }
    }

    PluginInstance* PluginManager::SeizePlugin(int pluginId, int agentType)
    {
        lock_guard<mutex> lock(pluginLock);
        bool found = false;

        for(int at : allowedAgentTypes[pluginId])
        {
            if(at == agentType)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            throw UnautorizedAccessException((char*)"This type of agent is not allowed to seize this plugin");
        }

        if(types[pluginId] == NONSHAREABLE)
        {
            if(statuses[pluginId] == SEIZED)
            {
                throw UnautorizedAccessException((char*)"This plugin is seized by another transaction");
            }            
            
            statuses[pluginId] = SEIZED;
            return instances[pluginId];
        }

        return instances[pluginId];

        /*
        if(types[pluginId] == NONSHAREABLE)
        {
            if(statuses[pluginId] == SEIZED)
            {
                throw UnautorizedAccessException((char*)"This plugin is seized by another transaction");
            }            
            
            PluginInstance* res = instancePools[pluginId]->GetInstance();
            statuses[pluginId] = SEIZED;
            return res;
        }

        return instancePools[pluginId]->GetInstance();
        */
    }

    void PluginManager::ReleasePlugin(PluginInstance* instance)
    {
        lock_guard<mutex> lock(pluginLock);
        statuses[instance->pluginId] = FREE;

        /*
        lock_guard<mutex> lock(pluginLock);
        instancePools[instance->pluginId]->ReturnInstance(instance);
        statuses[instance->pluginId] = FREE;
        */
    }

    const set<int> PluginManager::GetNonShareable(const set<int>& plugins)
    {
        set<int> res;

        for(int p : plugins)
        {
            if(types[p] == NONSHAREABLE)
            {
                res.insert(p);
            }
        }

        return res;
    }

    const set<int>& PluginManager::GetNonShareable()
    {
        return nonShareable;
    }

    PluginManager::~PluginManager()
    {
        for(auto p : instances)
        {
            delete p.second;
        }

        /*
        for(auto p : instancePools)
        {
            delete p.second;
        }
            */
    }
}