#include "random_utils.h"
#include "task_agents.h"

namespace angonoka::stun {
RandomUtils::RandomUtils(gsl::not_null<const TaskAgents*> task_agents)
    : task_agents{std::move(task_agents)}
{
}
float RandomUtils::get_uniform() noexcept { return r(g); }

index RandomUtils::random_index(index max) noexcept
{
    return static_cast<index>(r(g) * static_cast<float>(max));
}

int16 RandomUtils::pick_random(span<const int16> rng) noexcept
{
    return rng[random_index(rng.size())];
}

void RandomUtils::get_neighbor_inplace(span<int16> v) noexcept
{
    const auto task_idx = random_index(v.size());
    v[task_idx] = pick_random((*task_agents)[task_idx]);
}
} // namespace angonoka::stun
