#pragma once
#include <Shared/ISharedPointerWrapper.h>

namespace OptiMA
{
    template <typename T>
    class SharedPointerWrapper : public ISharedPointerWrapper
    {
        shared_ptr<T> ptr;

    public:

        SharedPointerWrapper(shared_ptr<T> object) : ptr(object) {}

        const std::type_info& Type() override
        {
            return typeid(T);
        }
        
        shared_ptr<void> GetPtr() override
        {
            return std::static_pointer_cast<void>(ptr);
        }
    };
}