#pragma once
#include <exception>
#include <string>

namespace OptiMA
{
    class OptiMAException : public std::exception
    {
    public:       

        const char* what() const noexcept override;

    protected:

        std::string errorMessage_;

        OptiMAException(const std::string& errorMessage);
    
    };

    class UnautorizedAccessException : public OptiMAException
    {
    public:

        UnautorizedAccessException(const std::string& errorMessage);
    };

    class AgentLimitException : public OptiMAException
    {
    public:

        AgentLimitException(const std::string& errorMessage);
    };

    class AgentUnavailableException : public OptiMAException
    {
    public:
    
        AgentUnavailableException(const std::string& errorMessage);
    };

    class PluginLimitException : public OptiMAException
    {
    public:

        PluginLimitException(const std::string& errorMessage);
    };

    class InvalidModelParameterException : public OptiMAException
    {
    public:

        InvalidModelParameterException(const std::string& errorMessage);
    };

    class UserAbortException : public OptiMAException
    {
    public:
        
        UserAbortException(const std::string& errorMessage);
    };
}