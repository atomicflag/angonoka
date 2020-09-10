#pragma once

#include <cstdint>

namespace angonoka::stun {
    using int16 = std::int_fast16_t;
    constexpr std::uint_fast64_t max_iterations = 10'000'000u;
} // namespace angonoka::stun
