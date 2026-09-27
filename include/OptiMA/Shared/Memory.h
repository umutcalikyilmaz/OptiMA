#pragma once
#include "OptiMA/Shared/TupleWrapper.h"

namespace OptiMA
{
    class Memory
    {
    public:
        
        template <typename...Args>
        void addTuple(std::tuple<Args...> record)
        {
            tuples_.push_back(std::make_shared<TupleWrapper<Args...>>(record));
        }

        template <typename...Args>
        void addTuple(Args... parameters)
        {
            tuples_.push_back(std::make_shared<TupleWrapper<Args...>>(std::forward<Args>(parameters)...));
        }

        template <typename...Args>
        std::tuple<Args...>& getTuple(int index)
        {
            return tuples_[index]->getTuple<Args...>();
        }

        template <typename...Args>
        const std::tuple<Args...>& getTuple(int index) const
        {
            return tuples_[index]->getTuple<Args...>();
        }

        int getTupleCount() const
        {
            return tuples_.size();
        }
        
    private:
    
        std::vector<std::shared_ptr<ITupleWrapper>> tuples_;
    };
}