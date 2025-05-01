#pragma once
#include <Shared/Exceptions.h>
#include <Shared/SharedPointerWrapper.h>
#include <Shared/Types.h>
/*
namespace OptiMA
{
    template <typename T>
    struct is_shared_ptr : std::false_type {};

    template <typename T>
    struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

    template <typename T>
    struct shared_ptr_type 
    {
        using type = void; 
    };

    template <typename T>
    struct shared_ptr_type<std::shared_ptr<T>> 
    {
        using type = T;
    };
    
    class Record final
    {
        map<int,AgentStatus> statuses;
        map<int,bool> bools;
        map<int,int> ints;
        map<int,long> longs;
        map<int,double> doubles;
        map<int,string> strings;
        map<int,unique_ptr<ISharedPointerWrapper>> sharedPointers;
        vector<ParameterType> types;
        int count;

    public:

        Record() : count(0) { }

        Record(const Record& record)
        {
            if(!record.bools.empty())
            {
                for(auto p : record.bools)
                {
                    bools[p.first] = p.second;
                }
            }

            if(!record.ints.empty())
            {
                for(auto p : record.ints)
                {
                    ints[p.first] = p.second;
                }
            }

            if(!record.longs.empty())
            {
                for(auto p : record.longs)
                {
                    longs[p.first] = p.second;
                }
            }

            if(!record.doubles.empty())
            {
                for(auto p : record.doubles)
                {
                    doubles[p.first] = p.second;
                }
            }

            if(!record.strings.empty())
            {
                for(auto p : record.strings)
                {
                    strings[p.first] = p.second;
                }
            }
        }

        template<typename T, typename = enable_if_t <is_same_v<T, bool> || is_same_v<T, int> ||
        is_same_v<T, double> || is_same_v<T, long> || is_same_v<T, string> || is_same_v<T, AgentStatus> || is_shared_ptr<T>::value >>
        void InsertParameter(T& param)
        {
            if constexpr(is_same_v<decay_t<T>,AgentStatus>)
            {
                statuses[count] = param;
                types.push_back(AGENTSTATUS);
            }
            else if constexpr(is_same_v<decay_t<T>,bool>)
            {
                bools[count] = param;
                types.push_back(BOOL);
            }
            else if constexpr(is_same_v<decay_t<T>,int>)
            {
                ints[count] = param;
                types.push_back(INT);
            }
            else if constexpr(is_same_v<decay_t<T>,double>)
            {
                doubles[count] = param;
                types.push_back(DOUBLE);
            }
            else if constexpr(is_same_v<decay_t<T>,long>)
            {
                longs[count] = param;
                types.push_back(LONG);
            }
            else if constexpr(is_same_v<decay_t<T>,string>)
            {
                strings[count] = param;
                types.push_back(STRING);
            }
            else if constexpr(is_shared_ptr<decay_t<T>>::value)
            {
                using pointerType = typename shared_ptr_type<T>::type;
                sharedPointers[count] = make_unique<SharedPointerWrapper<pointerType>>(param);
                types.push_back(SHAREDPOINTER);
            }

            count++;
        }

        template<typename T, typename = enable_if< is_same_v<T, bool> || is_same_v<T, int> ||
        is_same_v<T, double> || is_same_v<T, long> || is_same_v<T, const string&> || is_same_v<T, AgentStatus> || is_shared_ptr<T>::value> >
        T RetrieveParameter(int index)
        {
            if constexpr(is_same_v<decay_t<T>,AgentStatus>)
            {
                return statuses.at(index);
            }
            if constexpr(is_same_v<T,bool>)
            {
                return bools.at(index);
            }
            else if constexpr(is_same_v<T,int>)
            {
                return ints.at(index);
            }
            else if constexpr(is_same_v<T,double>)
            {
                return doubles.at(index);
            }
            else if constexpr(is_same_v<T,long>)
            {
                return longs.at(index);
            }
            else if constexpr(is_same_v<T,string>)
            {
                return strings.at(index);
            }
            else if constexpr(is_shared_ptr<decay_t<T>>::value)
            {
                using pointerType = typename shared_ptr_type<T>::type;
                return sharedPointers.at(index)->GetSharedPointer<pointerType>();
            }
        }
    
        const vector<ParameterType>& GetParameterTypes()
        {
            return types;
        }

        int GetParameterNumber()
        {
            return count;
        }
    };
}
*/