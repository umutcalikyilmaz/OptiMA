#include "OptiMA/Shared/Exceptions.h"

namespace OptiMA
{
    OptiMAException::OptiMAException(const std::string& errorMessage) : errorMessage_(errorMessage) { }

    const char* OptiMAException::what() const noexcept
    {
        return errorMessage_.c_str();
    };

    UnautorizedAccessException::UnautorizedAccessException(const std::string& errorMessage) : OptiMAException(errorMessage) { }

    AgentLimitException::AgentLimitException(const std::string& errorMessage) : OptiMAException(errorMessage) { }

    AgentUnavailableException::AgentUnavailableException(const std::string& errorMessage) : OptiMAException(errorMessage) { }

    PluginLimitException::PluginLimitException(const std::string& errorMessage) : OptiMAException(errorMessage) { }

    InvalidModelParameterException::InvalidModelParameterException(const std::string& errorMessage) : OptiMAException(errorMessage) { }

    UserAbortException::UserAbortException(const std::string& errorMessage) : OptiMAException(errorMessage) { }
}