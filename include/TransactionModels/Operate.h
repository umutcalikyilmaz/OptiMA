#pragma once
#include <tuple>
#include <Engine/PluginManager.h>
#include <TransactionModels/ProceduralTransaction.h>

namespace OptiMA
{
    template <typename A, typename... Args>
    class Operate : public ProceduralTransaction
    {
        static_assert(is_base_of<AgentTemplate<decay_t<A>>, decay_t<A>>::value, "A member function of a child class of AgentTemplate is required");

        using AgentFunction = shared_ptr<Memory>(A::*)(Args...);
        A* functionOwner;
        AgentFunction operateFunction;
        tuple<Args...> arguments;

    public:

        Operate(Agent* owner, set<int>& nonShareablePlugins, A* functionOwner, AgentFunction operateFunction, int subType,
        Args... arguments) : ProceduralTransaction(owner, OPERATE, subType, nonShareablePlugins), functionOwner(functionOwner),
        operateFunction(operateFunction), arguments(make_tuple(forward<Args>(arguments)...))
        {
            this->subType = subType;
        }

        unique_ptr<TransactionResult> Procedure() override final
        {
            shared_ptr<Memory> output;

            try
            {
                try
                {                    
                    output = std::apply([this](Args... unpacked_args) {
                        return (functionOwner->*operateFunction)(std::forward<Args>(unpacked_args)...);
                    }, arguments);                    
                }
                catch(UserAbortException e)
                {
                    return make_unique<TransactionResult>(ABORTED, e.what());
                }
            }
            catch(exception e)
            {
                return make_unique<TransactionResult>(FAILED, e.what());
            }

            return make_unique<TransactionResult>(SUCCESSFUL, output);
        }

        virtual ~Operate() = default;
    };

    template <typename A, typename... Args>
    Operate(int (A::*)(Args...), A*, Args...) -> Operate<A, Args...>;

    template <typename A, typename... Args>
    Operate(int (A::*)(Args...) const, A*, Args...) -> Operate<A, Args...>;
}