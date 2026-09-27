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

        class DriverKey
        {
        private:
            
            DriverKey() {}

            friend class Driver;
        };

        class AgentKey
        {
        private:

            AgentKey() {}

            template<class A>
            friend class AgentTemplate;
        };

        PluginManager(DriverKey, const std::vector<std::unique_ptr<IPluginInstanceFactory>>& factories,
            const std::vector<PluginType>& pluginTypes, const std::vector<int>& pluginIds,
            const std::vector<std::pair<int,int>>& pluginAccesses);
        
        PluginInstance* seizePlugin(AgentKey, int pluginId, int agentType);

        void releasePlugin(AgentKey, PluginInstance* instance);

        const std::set<int> getNonShareable(const std::set<int>& plugins);

        const std::set<int>& getNonShareable();

    private:

        std::map<int, std::unique_ptr<PluginInstance>> instances_;
        std::map<int, PluginType> types_;
        std::map<int, PluginStatus> statuses_;
        std::map<int, std::vector<int>> allowedAgentTypes_;
        std::set<int> nonShareable_;
        std::mutex pluginLock_;
    };
}