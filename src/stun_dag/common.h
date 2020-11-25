#pragma once

#include <cstdint>
#include <fmt/format.h>
#include <range/v3/view/span.hpp>
#ifndef NDEBUG
#include <boost/safe_numerics/safe_integer.hpp>
#include <iosfwd>
#endif // NDEBUG

namespace angonoka::stun_dag {
#ifndef NDEBUG
namespace sn = boost::safe_numerics;
using int16 = sn::safe<std::int_fast16_t>;
using uint32 = sn::safe<std::uint_fast32_t>;
using uint64 = sn::safe<std::uint_fast64_t>;
#else // NDEBUG
using int16 = std::int_fast16_t;
using uint32 = std::uint_fast32_t;
using uint64 = std::uint_fast64_t;
#endif // NDEBUG

/**
    Scheduling information for the task.

    The position of StateItem within State dictates
    the relative order of this task.

    @var task_id    Task's index
    @var agent_id   Agent's index
*/
struct StateItem {
    int16 task_id;
    int16 agent_id;

    bool operator==(const StateItem&) const = default;

// Needed for Catch diagnostics
#ifndef NDEBUG
    friend std::ostream&
    operator<<(std::ostream& os, const StateItem& item);
#endif // NDEBUG
};

using State = ranges::span<const StateItem>;
using MutState = ranges::span<StateItem>;
} // namespace angonoka::stun_dag

namespace fmt {
using angonoka::stun_dag::StateItem;
template <> struct fmt::formatter<StateItem> {
    static constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.end();
    }
    template <typename FormatContext>
    constexpr auto format(const StateItem& item, FormatContext& ctx)
    {
        return format_to(
            ctx.out(),
            "({}, {})",
            static_cast<int>(item.task_id),
            static_cast<int>(item.agent_id));
    }
};
} // namespace fmt
