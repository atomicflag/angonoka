#include "system.h"
#include "exceptions.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/algorithm/any_of.hpp>

namespace angonoka {
bool Agent::is_universal() const noexcept
{
    return group_ids.empty();
}

bool Agent::can_work_on(GroupId id) const noexcept
{
    Expects(id >= 0);

    if (is_universal()) return true;
    return group_ids.contains(id);
}

bool System::has_universal_agents() const noexcept
{
    Expects(!agents.empty());

    return ranges::any_of(agents, &Agent::is_universal);
}

Agent::Performance::Value::Value(float v)
{
    if (v > 0.F) {
        value = v;

        return;
    }
    throw NegativePerformance{};
}

Agent::Performance::Value::operator float() const noexcept
{
    return value;
}
} // namespace angonoka
