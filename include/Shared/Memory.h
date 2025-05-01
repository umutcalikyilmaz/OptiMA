#pragma once
#include <Shared/TupleWrapper.h>

namespace OptiMA
{
    class Memory
    {
    public:
        
        template <typename...Args>
        void addTuple(tuple<Args...> record)
        {
            tuples.push_back(make_shared<TupleWrapper<Args...>>(record));
        }

        template <typename...Args>
        void addTuple(Args... parameters)
        {
            tuples.push_back(make_shared<TupleWrapper<Args...>>(forward<Args>(parameters)...));
        }

        template <typename...Args>
        tuple<Args...>& getTuple(int index)
        {
            return tuples[index]->getTuple<Args...>();
        }

        template <typename...Args>
        const tuple<Args...>& getTuple(int index) const
        {
            return tuples[index]->getTuple<Args...>();
        }

        int getTupleCount() const
        {
            return tuples.size();
        }
        
    private:
    
        vector<shared_ptr<ITupleWrapper>> tuples;
    };
}


/*
#pragma once
#include <Shared/Exceptions.h>
#include <Shared/SharedPointerWrapper.h>
#include <Shared/Types.h>


namespace OptiMA
{
    template<typename... Args>
    class Memory
    {

        map<int,shared_ptr<Record>> records;

        bool CheckRecords(int recordId);

    public:

        bool AddRecord(int recordId);

        bool AddRecord(int recordId, shared_ptr<Record> record);

        bool ReplaceRecord(int recordId);

        bool ReplaceRecord(int recordId, shared_ptr<Record> record);

        bool RemoveRecord(int recordId);

        shared_ptr<Record> GetRecord(int recordId);

        map<int,shared_ptr<Record>>& GetRecordMap();

        ~Memory();
    };
}
*/