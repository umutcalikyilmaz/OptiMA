#include "OptiMA/Engine/PluginManager.h"

namespace OptiMA
{
    PluginManager::PluginManager(vector<IPluginInstanceFactory*>& factories, vector<PluginType> pluginTypes,
    vector<int> pluginIds, vector<pair<int,int>> pluginAccesses)
    {
        int c = 0;

        for(int id : pluginIds)
        {
            instances_[id] = factories[c]->createPluginInstance();
            instances_[id]->pluginId_ = id;
            types_[id] = pluginTypes[c];
            statuses_[id] = FREE;
            allowedAgentTypes_[id] = vector<int>();

            if(pluginTypes[c] == NONSHAREABLE)
            {
                nonShareable_.insert(id);
            }

            c++;
        }

        for(pair<int,int> p : pluginAccesses)
        {
            allowedAgentTypes_[p.second].push_back(p.first);
        }
    }

    PluginInstance* PluginManager::seizePlugin(int pluginId, int agentType)
    {
        lock_guard<mutex> lock(pluginLock_);
        bool found = false;

        for(int at : allowedAgentTypes_[pluginId])
        {
            if(at == agentType)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            throw UnautorizedAccessException("This type of agent is not allowed to seize this plugin");
        }

        if(types_[pluginId] == NONSHAREABLE)
        {
            if(statuses_[pluginId] == SEIZED)
            {
                throw UnautorizedAccessException("This plugin is seized by another transaction");
            }            
            
            statuses_[pluginId] = SEIZED;
            return instances_[pluginId];
        }

        return instances_[pluginId];
    }

    void PluginManager::releasePlugin(PluginInstance* instance)
    {
        lock_guard<mutex> lock(pluginLock_);
        statuses_[instance->pluginId_] = FREE;
    }

    const set<int> PluginManager::getNonShareable(const set<int>& plugins)
    {
        set<int> res;

        for(int p : plugins)
        {
            if(types_[p] == NONSHAREABLE)
            {
                res.insert(p);
            }
        }

        return res;
    }

    const set<int>& PluginManager::getNonShareable()
    {
        return nonShareable_;
    }

    PluginManager::~PluginManager()
    {
        for(auto p : instances_)
        {
            delete p.second;
        }
    }
}