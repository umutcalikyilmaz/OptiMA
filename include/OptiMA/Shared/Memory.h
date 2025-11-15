#pragma once
#include "OptiMA/Shared/TupleWrapper.h"

namespace OptiMA
{
    class Memory
    {
    public:
        
        template <typename...Args>
        void addTuple(tuple<Args...> record)
        {
            tuples_.push_back(make_shared<TupleWrapper<Args...>>(record));
        }

        template <typename...Args>
        void addTuple(Args... parameters)
        {
            tuples_.push_back(make_shared<TupleWrapper<Args...>>(forward<Args>(parameters)...));
        }

        template <typename...Args>
        tuple<Args...>& getTuple(int index)
        {
            return tuples_[index]->getTuple<Args...>();
        }

        template <typename...Args>
        const tuple<Args...>& getTuple(int index) const
        {
            return tuples_[index]->getTuple<Args...>();
        }

        int getTupleCount() const
        {
            return tuples_.size();
        }
        
    private:
    
        vector<shared_ptr<ITupleWrapper>> tuples_;
    };
}