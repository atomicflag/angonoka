#pragma once

#include <cstdint>
#include <range/v3/view/span.hpp>
#ifndef NDEBUG
#include <boost/safe_numerics/automatic.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#endif

namespace angonoka::stun {
#ifndef NDEBUG
namespace sn = boost::safe_numerics;
using uint64 = sn::safe<std::uint_fast64_t, sn::automatic>;
using uint32 = sn::safe<std::int_fast32_t, sn::automatic>;
using int16 = sn::safe<std::int_fast16_t, sn::automatic>;
#else
using uint64 = std::uint_fast64_t;
using uint32 = std::uint_fast32_t;
using int16 = std::int_fast16_t;
#endif
constexpr uint64 default_max_iterations = 10'000'000U;
#ifdef NDEBUG
constexpr uint64 max_iterations = default_max_iterations;
#else // NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern uint64 max_iterations;
#endif // NDEBUG
using index = ranges::span<int>::index_type;
} // namespace angonoka::stun
