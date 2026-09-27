#pragma once
#include "OptiMA/Shared/ITupleWrapper.h"

namespace OptiMA
{
    template <typename... Args>
    class TupleWrapper : public ITupleWrapper
    {
    public:

        TupleWrapper(Args... parameters) : record_(std::make_tuple(std::forward<Args>(parameters)...)) { }

        TupleWrapper(const std::tuple<Args...>& record) : record_(record) { }

    private:

        std::tuple<Args...> record_;

        const std::type_info& getType() const override
        {
            return typeid(std::tuple<Args...>);
        }

        const void* getPointer() const override
        {
            return &record_; 
        }
    };
}