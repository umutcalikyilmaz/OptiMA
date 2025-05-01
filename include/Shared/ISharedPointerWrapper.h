#pragma once
#include <Shared/Types.h>

namespace OptiMA
{
    class ISharedPointerWrapper
    {
    protected:

        virtual std::shared_ptr<void> GetPtr() = 0;

    public:

        virtual const std::type_info& Type() = 0;
        
        template <typename T>
        shared_ptr<T> GetSharedPointer()
        {
            if(Type() != typeid(T))
            {
                throw bad_cast();
            }

            return std::static_pointer_cast<T>(GetPtr());
        }

        virtual ~ISharedPointerWrapper() = default;
    };
}