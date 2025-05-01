#include <Shared/Memory.h>

/*
namespace OptiMA
{
    bool Memory::CheckRecords(int recordId)
    {
        return records.find(recordId) != records.end();
    }
    
    bool Memory::AddRecord(int recordId)
    {
        if(CheckRecords(recordId))
        {
            return false;
        }

        records[recordId] = make_shared<Record>();
        return true;
    }

    bool Memory::AddRecord(int recordId, shared_ptr<Record> record)
    {
        if(CheckRecords(recordId))
        {
            return false;
        }

        records[recordId] = record;
        return true;
    }

    shared_ptr<Record> Memory::GetRecord(int recordId)
    {
        auto it = records.find(recordId);

        if(it == records.end())
        {
            return nullptr;
        }
        
        return it->second;
    }

    bool Memory::ReplaceRecord(int recordId)
    {
        auto it = records.find(recordId);

        if(it == records.end())
        {
            return false;
        }

        it->second = make_shared<Record>();
        return true;
    }

    bool Memory::ReplaceRecord(int recordId, shared_ptr<Record> record)
    {
        auto it = records.find(recordId);

        if(it == records.end())
        {
            return false;
        }

        it->second = record;
        return true;
    }

    bool Memory::RemoveRecord(int recordId)
    {
        auto it = records.find(recordId);

        if(it == records.end())
        {
            return false;
        }

        records.erase(recordId);
        return true;
    }

    map<int,shared_ptr<Record>>& Memory::GetRecordMap()
    {
        return records;
    }

    Memory::~Memory()
    {
        records.clear();
    }
}

*/