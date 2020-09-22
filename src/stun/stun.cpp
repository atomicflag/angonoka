#include "beta_driver.h"
#include "makespan_estimator.h"
#include "random_utils.h"
#include "task_agents.h"
#include "task_durations.h"
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <utility>

namespace angonoka::stun {
RandomUtils::RandomUtils(gsl::not_null<const TaskAgents*> task_agents)
    : task_agents{std::move(task_agents)}
{
}
float RandomUtils::get_uniform() noexcept { return r(g); }

RandomUtils::index_type
RandomUtils::random_index(index_type max) noexcept
{
    return static_cast<index_type>(r(g) * static_cast<float>(max));
}

int16 RandomUtils::pick_random(span<const int16> rng) noexcept
{
    return rng[random_index(rng.size())];
}

void RandomUtils::get_neighbor(span<int16> v) noexcept
{
    const auto task_idx = random_index(v.size());
    v[task_idx] = pick_random((*task_agents)[task_idx]);
}

TaskDurations::TaskDurations(
    span<const float> task_durations,
    span<const float> agent_performances)
    : float_data{std::make_unique<float[]>(
        task_durations.size() * agent_performances.size())}
    , agent_count{agent_performances.size()}
{
    for (gsl::index i{0}; i < task_durations.size(); ++i) {
        for (gsl::index j{0}; j < agent_performances.size(); ++j) {
            const auto idx = build_index(AgentIndex{j}, TaskIndex{i});
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
