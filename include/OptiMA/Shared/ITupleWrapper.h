#pragma once
#include "OptiMA/Shared/Exceptions.h"
#include "OptiMA/Shared/Types.h"

namespace OptiMA
{
    class ITupleWrapper
    {
    public:
        
        template<typename... Args>
        std::tuple<Args...>& getTuple()
        {
            if(getType() != typeid(std::tuple<Args...>))
            {
                throw std::bad_cast();
            }

            return *static_cast<std::tuple<Args...>*>(const_cast<void*>(getPointer()));
        }

        
        template<typename... Args>
        const std::tuple<Args...>& getTuple() const
        {
            if(getType() != typeid(std::tuple<Args...>))
            {
                throw std::bad_cast();
            }

            return *static_cast<std::tuple<Args...>*>(getPointer());
        }

        virtual ~ITupleWrapper() = default;    

    protected:
    
        virtual const std::type_info& getType() const = 0;

        virtual const void* getPointer() const = 0;
    };
}