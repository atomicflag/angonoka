#include "random_utils.h"

namespace angonoka::stun_dag {
// TODO: Update
// RandomUtils::RandomUtils(
//     gsl::not_null<const TaskAgentsT*> task_agents)
//     : task_agents{std::move(task_agents)}
// {
// }

// TODO: Update
// RandomUtils::RandomUtils(
//     gsl::not_null<const TaskAgentsT*> task_agents,
//     gsl::index seed)
//     : task_agents{std::move(task_agents)}
//     , generator{seed}
// {
// }

float RandomUtils::get_uniform() noexcept
{
    return uniform(generator);
}
} // namespace angonoka::stun_dag
