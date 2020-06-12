#include "system.h"
#include "exceptions.h"
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

Agent::Performance::Value::Value(float v)
{
    if (v >= 0.F && v <= 100.F) {
        value = v;
        return;
    }
    throw ValidationError{
        "Agent performance must be between 0.0 and 100.0"};
}

Agent::Performance::Value::operator float() const noexcept
{
    return value;
}
} // namespace angonoka
