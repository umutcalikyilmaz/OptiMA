#include <Shared/Exceptions.h>

namespace OptiMA
{
    MultiAgentException::MultiAgentException(char* errorMessage) : errorMessage(errorMessage) { }

    char* MultiAgentException::what()
    {
        return errorMessage;
    };

    UnautorizedAccessException::UnautorizedAccessException(char* errorMessage) : MultiAgentException(errorMessage) { }

    AgentLimitException::AgentLimitException(char* errorMessage) : MultiAgentException(errorMessage) { }

    PluginLimitException::PluginLimitException(char* errorMessage) : MultiAgentException(errorMessage) { }

    InvalidModelParameterException::InvalidModelParameterException(char* errorMessage) : MultiAgentException(errorMessage) { }

    UserAbortException::UserAbortException(char* errorMessage) : MultiAgentException(errorMessage) { }
}