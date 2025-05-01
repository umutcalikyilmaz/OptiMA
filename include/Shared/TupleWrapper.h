#pragma once
#include <Shared/ITupleWrapper.h>

namespace OptiMA
{
    template <typename... Args>
    class TupleWrapper : public ITupleWrapper
    {
    private:
        tuple<Args...> record_;

        const type_info& getType() const override
        {
            return typeid(tuple<Args...>);
        }

        const void* getPointer() const override
        {
            return &record_; 
        }

    public:

        TupleWrapper(Args... parameters) : record_(make_tuple(forward<Args>(parameters)...)) { }

        TupleWrapper(const tuple<Args...>& record) : record_(record) { }
    };
}