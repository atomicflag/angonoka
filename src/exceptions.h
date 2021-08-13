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
struct DurationParseError : std::exception {
    DurationParseError(std::string_view text);
    std::string_view text;
};
struct InvalidDuration : ValidationError {
    InvalidDuration(std::string_view where, std::string_view what);
};
struct SchemaError : ValidationError {
    using ValidationError::ValidationError;
};
struct InvalidAgentPerformance : ValidationError {
    InvalidAgentPerformance(std::string_view who);
};
struct InvalidTaskAssignment : ValidationError {
    InvalidTaskAssignment(std::string_view task);
};
struct AgentPerformanceMinMax : ValidationError {
    AgentPerformanceMinMax(std::string_view who);
};
struct DuplicateAgentDefinition : ValidationError {
    DuplicateAgentDefinition(std::string_view who);
};
struct TaskDurationMinMax : ValidationError {
    TaskDurationMinMax(std::string_view where);
};
struct NoSuitableAgent : ValidationError {
    NoSuitableAgent(std::string_view task);
};
struct DuplicateTaskDefinition : ValidationError {
    DuplicateTaskDefinition(std::string_view task_id);
};
struct NegativePerformance : ValidationError {
    NegativePerformance(std::string_view who);
};
struct CantBeEmpty : ValidationError {
    CantBeEmpty(std::string_view what);
};
struct TaskNotFound : ValidationError {
    TaskNotFound(std::string_view task_id);
};
struct AgentNotFound : ValidationError {
    AgentNotFound(std::string_view name);
};
struct DependencyCycle : ValidationError {
    DependencyCycle();
};
} // namespace angonoka
