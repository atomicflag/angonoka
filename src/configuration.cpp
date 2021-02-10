#include "configuration.h"
#include "exceptions.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/algorithm/any_of.hpp>

namespace angonoka {
bool is_universal(const Agent& agent) noexcept
{
    return agent.group_ids.empty();
}

bool can_work_on(const Agent& agent, GroupIndex id) noexcept
{
    Expects(id >= 0);

    if (is_universal(agent)) return true;
    return agent.group_ids.contains(id);
}

[[nodiscard]] bool
can_work_on(const Agent& agent, const Task& task) noexcept
{
    return is_universal(agent)
        || (task.group_id && can_work_on(agent, *task.group_id));
}

bool has_universal_agents(const Configuration& config) noexcept
{
    Expects(!config.agents.empty());

    return ranges::any_of(config.agents, is_universal);
}

float Agent::Performance::average() const
{
    Expects(min > 0.F);
    Expects(min <= max);
    // NOLINTNEXTLINE: cppcoreguidelines-avoid-magic-numbers
    return (min + max) / 2.F;
}

std::chrono::seconds Task::Duration::average() const
{
    Expects(min > std::chrono::seconds{0});
    Expects(min <= max);
    // NOLINTNEXTLINE: cppcoreguidelines-avoid-magic-numbers
    return (min + max) / 2;
}
} // namespace angonoka
