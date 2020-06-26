#include "exceptions.h"
#include <fmt/format.h>

namespace angonoka {

InvalidDuration::InvalidDuration()
    : ValidationError{"Invalid duration."}
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
TaskDurationMinMax::TaskDurationMinMax()
    : ValidationError{
        "Task's duration minimum can't be greater than maximum."}
{
}
NoSuitableAgent::NoSuitableAgent(std::string_view task)
    : ValidationError{
        fmt::format(R"_(No suitable agent for task "{}")_", task)}
{
}
DuplicateTaskDefinition::DuplicateTaskDefinition()
    : ValidationError{"Duplicate task definition"}
{
}
NegativePerformance::NegativePerformance()
    : ValidationError{"Agent's performance must be greater than 0"}
{
}
} // namespace angonoka
