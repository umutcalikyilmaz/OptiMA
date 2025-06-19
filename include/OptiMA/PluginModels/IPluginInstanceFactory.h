#pragma once
#include "OptiMA/PluginModels/Plugin.h"

namespace OptiMA
{
    class IPluginInstanceFactory
    {
    public:
        
        virtual PluginInstance* createPluginInstance() = 0;
    };
}