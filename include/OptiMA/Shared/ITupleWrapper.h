#pragma once
#include "OptiMA/Shared/Exceptions.h"
#include "OptiMA/Shared/Types.h"

namespace OptiMA
{
    class ITupleWrapper
    {
    protected:
    
        virtual const type_info& getType() const = 0;

        virtual const void* getPointer() const = 0;

    public:
        
        template<typename... Args>
        tuple<Args...>& getTuple()
        {
            if(getType() != typeid(tuple<Args...>))
            {
                throw bad_cast();
            }

            return *static_cast<tuple<Args...>*>(const_cast<void*>(getPointer()));
        }

        
        template<typename... Args>
        const tuple<Args...>& getTuple() const
        {
            if(getType() != typeid(tuple<Args...>))
            {
                throw bad_cast();
            }

            return *static_cast<tuple<Args...>*>(getPointer());
        }

        virtual ~ITupleWrapper() = default;    
    };
}