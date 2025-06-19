#pragma once
#include <atomic>
#include <mutex>
#include <thread>
#include "OptiMA/PluginModels/PluginInstanceFactory.h"

namespace OptiMA
{
    class PluginManager
    {
    public:

        PluginManager(vector<IPluginInstanceFactory*>& factories, vector<PluginType> pluginTypes,
        vector<int> pluginIds, vector<pair<int,int>> pluginAccesses);
        
        PluginInstance* seizePlugin(int pluginId, int agentType);

        void releasePlugin(PluginInstance* instance);

        const set<int> getNonShareable(const set<int>& plugins);

        const set<int>& getNonShareable();

        ~PluginManager();

    private:

        map<int,PluginInstance*> instances_;
        map<int,PluginType> types_;
        map<int,PluginStatus> statuses_;
        map<int,vector<int>> allowedAgentTypes_;
        set<int> nonShareable_;
        mutex pluginLock_;
    };
}