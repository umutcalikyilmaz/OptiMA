#pragma once
#include "OptiMA/PluginModels/IPluginInstanceFactory.h"

namespace OptiMA
{
    template <typename T, typename = std::enable_if_t< std::is_base_of<Plugin<std::decay_t<T> >, std::decay_t<T>>::value>>
    class PluginInstanceFactory : public IPluginInstanceFactory
    {
        using PluginType = std::decay_t<T>;

    public:
        
        PluginInstance* createPluginInstance()
        {
            return (PluginInstance*)std::make_unique<PluginType>().release();
        }
    };
}