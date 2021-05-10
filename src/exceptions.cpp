#include "exceptions.h"
#include <fmt/format.h>

namespace angonoka {
using namespace fmt::literals;
DurationParseError::DurationParseError(std::string_view text)
    : text{text}
{
}
InvalidDuration::InvalidDuration(
    std::string_view where,
    std::string_view what)
    : ValidationError{
        R"(Task "{}" has invalid duration "{}".)"_format(where, what)}
{
}
InvalidAgentPerformance::InvalidAgentPerformance()
    : ValidationError{"Invalid agent performance."}
{
}
AgentPerformanceMinMax::AgentPerformanceMinMax()
    : ValidationError{
        "Agent's performance minimum can't be greater than maximum."}
{
}
DuplicateAgentDefinition::DuplicateAgentDefinition()
    : ValidationError{"Duplicate agent definition"}
{
}
TaskDurationMinMax::TaskDurationMinMax(std::string_view where)
    : ValidationError{
        R"(Task "{}" has min duration that is greater than max duration.)"_format(
            where)}
{
}
NoSuitableAgent::NoSuitableAgent(std::string_view task)
    : ValidationError{
        R"_(No suitable agent for task "{}")_"_format(task)}
{
}
DuplicateTaskDefinition::DuplicateTaskDefinition(
    std::string_view task_id)
    : ValidationError{R"(Duplicate task id "{}".)"_format(task_id)}
{
}
NegativePerformance::NegativePerformance()
    : ValidationError{"Agent's performance must be greater than 0"}
{
}

CantBeEmpty::CantBeEmpty(std::string_view what)
    : ValidationError{"{} can't be empty."_format(what)}
{
}

TaskNotFound::TaskNotFound(std::string_view task_id)
    : ValidationError{
        R"_(Task with id "{}" doesn't exist.)_"_format(task_id)}
{
}

DependencyCycle::DependencyCycle()
    : ValidationError{"Dependency cycle detected"}
{
}
} // namespace angonoka
