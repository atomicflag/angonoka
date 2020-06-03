#include "system.h"
#include <range/v3/algorithm/any_of.hpp>

namespace angonoka {
bool Agent::is_universal() const noexcept
{
    return group_ids.empty();
}

bool Agent::can_work_on(GroupId id) const noexcept
{
    if (is_universal()) return true;
    return group_ids.contains(id);
}

[[nodiscard]] bool System::has_universal_agents() const noexcept
{
    return ranges::any_of(agents, &Agent::is_universal);
}
} // namespace angonoka
