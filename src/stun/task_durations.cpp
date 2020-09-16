#include "task_durations.h"

namespace angonoka::stun {
TaskDurations::TaskDurations(
    span<const float> task_durations,
    span<const float> agent_performances)
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    : float_data{std::make_unique<float[]>(static_cast<std::size_t>(
        task_durations.size() * agent_performances.size()))}
    , agent_count{static_cast<gsl::index>(agent_performances.size())}
{
    using index = decltype(task_durations)::index_type;
    for (index i{0}; i < task_durations.size(); ++i) {
        for (index j{0}; j < agent_performances.size(); ++j) {
            const auto idx = build_index(
                static_cast<AgentIndex>(j),
                static_cast<TaskIndex>(i));
            float_data[idx]
                = task_durations[i] / agent_performances[j];
        }
    }
}

float TaskDurations::get(AgentIndex agent, TaskIndex task)
    const noexcept
{
    return float_data[build_index(agent, task)];
}

gsl::index TaskDurations::build_index(
    AgentIndex agent,
    TaskIndex task) const noexcept
{
    return static_cast<gsl::index>(task) * agent_count
        + static_cast<gsl::index>(agent);
}
} // namespace angonoka::stun
