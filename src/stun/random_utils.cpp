#include "random_utils.h"
#include "task_agents.h"

namespace angonoka::stun {
RandomUtils::RandomUtils(gsl::not_null<const TaskAgents*> task_agents)
    : task_agents{std::move(task_agents)}
{
}

RandomUtils::RandomUtils(
    gsl::not_null<const TaskAgents*> task_agents,
    gsl::index seed)
    : task_agents{std::move(task_agents)}
    , generator{seed}
{
}

float RandomUtils::get_uniform() noexcept
{
    return uniform(generator);
}

index RandomUtils::random_index(index max) noexcept
{
    Expects(max >= 0);

    const auto result
        = static_cast<index>(get_uniform() * static_cast<float>(max));

    Ensures(result < max);

    return result;
}

int16 RandomUtils::pick_random(span<const int16> range) noexcept
{
    Expects(!range.empty());

    return range[random_index(range.size())];
}

void RandomUtils::get_neighbor_inplace(span<int16> state) noexcept
{
    Expects(!state.empty());

    const auto task_idx = random_index(state.size());
    state[task_idx] = pick_random((*task_agents)[task_idx]);
}
} // namespace angonoka::stun
