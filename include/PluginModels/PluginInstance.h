#pragma once
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <Shared/SharedFactories.h>
#include <Engine/Postmaster.h>

namespace OptiMA
{
    class PluginInstance
    {
        PluginType type;
        int pluginId;

    public:

        virtual shared_ptr<Memory> Operate(shared_ptr<Memory> input) = 0;
        
        friend class InstancePool;
        friend class PluginManager;
    };
}