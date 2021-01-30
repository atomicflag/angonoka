#include "system.h"
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

bool has_universal_agents(const System& system) noexcept
{
    Expects(!system.agents.empty());

    return ranges::any_of(system.agents, is_universal);
}
} // namespace angonoka
