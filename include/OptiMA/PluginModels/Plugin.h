#pragma once
#include "OptiMA/PluginModels/PluginInstance.h"

namespace OptiMA
{
    template <class P> class Plugin : public PluginInstance
    {
    public:
    
        std::shared_ptr<Memory> operate(AgentKey, std::shared_ptr<Memory> inputParameters) override
        {
            return static_cast<P*>(this)->operate(inputParameters);
        }

    protected:

        Plugin() {}
    };
}