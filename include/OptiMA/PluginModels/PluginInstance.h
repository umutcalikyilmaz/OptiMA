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

        class AgentKey
        {
        private:

            AgentKey() {}

            template<class A>
            friend class AgentTemplate;
        };

        virtual std::shared_ptr<Memory> operate(AgentKey, std::shared_ptr<Memory> input) = 0;

    protected:

        PluginInstance() { }

    private:        

        PluginType type_;
        int pluginId_;

        friend class PluginManager;
    };
}