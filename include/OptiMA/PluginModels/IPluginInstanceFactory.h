#pragma once
#include "OptiMA/PluginModels/Plugin.h"

namespace OptiMA
{
    class IPluginInstanceFactory
    {
    public:
        
        virtual std::unique_ptr<PluginInstance> createPluginInstance() = 0;
    };
}