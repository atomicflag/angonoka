#include "task_duration_cache.h"

namespace angonoka::stun {
TaskDurationCache::TaskDurationCache(
    span<const float> task_durations,
    span<const float> agent_performances)
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    : float_data{std::make_unique<float[]>(static_cast<std::size_t>(
        task_durations.size() * agent_performances.size()))}
    , agent_count{static_cast<gsl::index>(agent_performances.size())}
{
    Expects(!task_durations.empty());
    Expects(!agent_performances.empty());

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

float TaskDurationCache::get(AgentIndex agent, TaskIndex task)
    const noexcept
{
    Expects(agent >= AgentIndex{0});
    Expects(task >= TaskIndex{0});
    Expects(agent < AgentIndex{agent_count});

    return float_data[build_index(agent, task)];
}

gsl::index TaskDurationCache::build_index(
    AgentIndex agent,
    TaskIndex task) const noexcept
{
    Expects(agent >= AgentIndex{0});
    Expects(task >= TaskIndex{0});
    Expects(agent < AgentIndex{agent_count});

    return static_cast<gsl::index>(task) * agent_count
        + static_cast<gsl::index>(agent);
}
} // namespace angonoka::stun
