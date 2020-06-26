#pragma once

#include <stdexcept>
#include <string_view>

namespace angonoka {

struct Exception : std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct ValidationError : Exception {
    using Exception::Exception;
};
struct InvalidDuration : ValidationError {
    InvalidDuration();
};
struct SchemaError : ValidationError {
    using ValidationError::ValidationError;
};
struct InvalidAgentPerformance : ValidationError {
    InvalidAgentPerformance();
};
struct AgentPerformanceMinMax : ValidationError {
    AgentPerformanceMinMax();
};
struct DuplicateAgentDefinition : ValidationError {
    DuplicateAgentDefinition();
};
struct TaskDurationMinMax : ValidationError {
    TaskDurationMinMax();
};
struct NoSuitableAgent : ValidationError {
    NoSuitableAgent(std::string_view task);
};
struct DuplicateTaskDefinition : ValidationError {
    DuplicateTaskDefinition();
};
struct NegativePerformance : ValidationError {
    NegativePerformance();
};

} // namespace angonoka
