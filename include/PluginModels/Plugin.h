#pragma once
#include <PluginModels/PluginInstance.h>

namespace OptiMA
{
    template <class P> class Plugin : public PluginInstance
    {
    public:
    
        shared_ptr<Memory> Operate(shared_ptr<Memory> inputParameters)
        {
            return static_cast<P*>(this)->Operate(inputParameters);
        }
    };
}