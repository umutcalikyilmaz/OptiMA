#pragma once
#include <atomic>
#include <mutex>
#include <thread>
#include <PluginModels/PluginInstanceFactory.h>

namespace OptiMA
{
    class PluginManager
    {
        map<int,PluginInstance*> instances;
        map<int,PluginType> types;
        map<int,PluginStatus> statuses;
        map<int,vector<int>> allowedAgentTypes;
        set<int> nonShareable;
        mutex pluginLock;

    public:

        PluginManager(vector<IPluginInstanceFactory*>& factories, vector<PluginType> pluginTypes,
        vector<int> pluginIds, vector<pair<int,int>> pluginAccesses);
        
        PluginInstance* SeizePlugin(int pluginId, int agentType);

        void ReleasePlugin(PluginInstance* instance);

        const set<int> GetNonShareable(const set<int>& plugins);

        const set<int>& GetNonShareable();

        ~PluginManager();
    };
}