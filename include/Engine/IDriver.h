#pragma once
#include <Shared/Memory.h>

namespace OptiMA
{
    class IDriver
    {
    public:

        virtual void HaltProgram(shared_ptr<Memory> outputParameters) = 0;

        virtual ~IDriver() = default;
    };
}