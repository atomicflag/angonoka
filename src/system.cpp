#include "system.h"

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
} // namespace angonoka
