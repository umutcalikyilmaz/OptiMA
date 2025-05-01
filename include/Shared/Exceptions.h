#pragma once
#include <exception>

namespace OptiMA
{
    class MultiAgentException : public std::exception
    {
    protected:

        char* errorMessage;

        MultiAgentException(char* errorMessage);

    public:       

        char* what();
    };

    class UnautorizedAccessException : public MultiAgentException
    {
    public:

        UnautorizedAccessException(char* errorMessage);
    };

    class AgentLimitException: public MultiAgentException
    {
    public:

        AgentLimitException(char* errorMessage);
    };

    class PluginLimitException: public MultiAgentException
    {
    public:

        PluginLimitException(char* errorMessage);
    };

    class InvalidModelParameterException : public MultiAgentException
    {
    public:

        InvalidModelParameterException(char* errorMessage);
    };

    class UserAbortException : public MultiAgentException
    {
    public:
        
        UserAbortException(char* errorMessage);
    };
}