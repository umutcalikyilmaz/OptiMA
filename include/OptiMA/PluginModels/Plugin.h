#pragma once
#include "OptiMA/PluginModels/PluginInstance.h"

namespace OptiMA
{
    template <class P> class Plugin : public PluginInstance
    {
    public:
    
        shared_ptr<Memory> operate(shared_ptr<Memory> inputParameters)
        {
            return static_cast<P*>(this)->operate(inputParameters);
        }
    };
}