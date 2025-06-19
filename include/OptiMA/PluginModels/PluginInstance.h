#pragma once
#include <memory>
#include <type_traits>
#include <typeinfo>
#include "OptiMA/Shared/SharedFactories.h"
#include "OptiMA/Engine/Postmaster.h"

namespace OptiMA
{
    class PluginInstance
    {
    public:

        virtual shared_ptr<Memory> operate(shared_ptr<Memory> input) = 0;
        
        friend class InstancePool;
        friend class PluginManager;

    private:

        PluginType type_;
        int pluginId_;
    };
}