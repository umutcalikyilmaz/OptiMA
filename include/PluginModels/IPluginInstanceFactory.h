#pragma once
#include <PluginModels/Plugin.h>

namespace OptiMA
{
    class IPluginInstanceFactory
    {
    public:
        
        virtual PluginInstance* CreatePluginInstance() = 0;
    };
}