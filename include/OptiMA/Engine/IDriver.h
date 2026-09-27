#pragma once
#include "OptiMA/Shared/Memory.h"

namespace OptiMA
{
    class IDriver
    {
    public:

        virtual void haltProgram(std::shared_ptr<Memory> outputParameters) = 0;

        virtual ~IDriver() = default;
    };
}