#pragma once

#include <cstdint>
#include <range/v3/view/span.hpp>
#ifndef NDEBUG
#include <boost/safe_numerics/automatic.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#endif

namespace angonoka::stun_dag {
#ifndef NDEBUG
namespace sn = boost::safe_numerics;
using int16 = sn::safe<std::int_fast16_t, sn::automatic>;
#else
using int16 = std::int_fast16_t;
#endif
struct StateItem {
    int16 task_id;
    int16 agent_id;
};
using State = ranges::span<StateItem>;
} // namespace angonoka::stun_dag