#include "OptiMA/Engine/PluginManager.h"

namespace OptiMA
{
    PluginManager::PluginManager(DriverKey, const std::vector<std::unique_ptr<IPluginInstanceFactory>>& factories,
        const std::vector<PluginType>& pluginTypes, const std::vector<int>& pluginIds,
        const std::vector<std::pair<int,int>>& pluginAccesses)
    {
        int c = 0;

        for(int id : pluginIds)
        {
            instances_[id] = factories[c]->createPluginInstance();
            instances_[id]->pluginId_ = id;
            types_[id] = pluginTypes[c];
            statuses_[id] = PluginStatus::FREE;
            allowedAgentTypes_[id] = std::vector<int>();

            if(pluginTypes[c] == PluginType::NONSHAREABLE)
            {
                nonShareable_.insert(id);
            }

            c++;
        }

        for(std::pair<int,int> p : pluginAccesses)
        {
            allowedAgentTypes_[p.second].push_back(p.first);
        }
    }

    PluginInstance* PluginManager::seizePlugin(AgentKey, int pluginId, int agentType)
    {
        std::lock_guard<std::mutex> lock(pluginLock_);
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

        if(types_[pluginId] == PluginType::NONSHAREABLE)
        {
            if(statuses_[pluginId] == PluginStatus::SEIZED)
            {
                throw UnautorizedAccessException("This plugin is seized by another transaction");
            }            
            
            statuses_[pluginId] = PluginStatus::SEIZED;
            return instances_[pluginId].get();
        }

        return instances_[pluginId].get();
    }

    void PluginManager::releasePlugin(AgentKey, PluginInstance* instance)
    {
        std::lock_guard<std::mutex> lock(pluginLock_);
        statuses_[instance->pluginId_] = PluginStatus::FREE;
    }

    const std::set<int> PluginManager::getNonShareable(const std::set<int>& plugins)
    {
        std::set<int> res;

        for(int p : plugins)
        {
            if(types_[p] == PluginType::NONSHAREABLE)
            {
                res.insert(p);
            }
        }

        return res;
    }

    const std::set<int>& PluginManager::getNonShareable()
    {
        return nonShareable_;
    }
}