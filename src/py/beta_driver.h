#pragma once

#include "common.h"
#include <cstdint>

// TODO: Docstrings
// This is like a stateful accumulator
// drv.update(...) - mutate
// drv.beta() - get new value

namespace angonoka::stun {
class BetaDriver {
public:
    BetaDriver(float beta, float beta_scale);

    // NOLINTNEXTLINE(bugprone-exception-escape)
    void update(float stun, std::uint_fast64_t iteration) noexcept;
    [[nodiscard]] float beta() const noexcept { return value; }
    [[nodiscard]] float last_average_stun() const noexcept
    {
        return last_average;
    }

private:
    float value;
    float average_stun{.0F};
    float last_average{.0F};
    std::uint_fast32_t stun_count{0};

    float beta_scale; // TODO: Temporary, should be hardcoded
};
} // namespace angonoka::stun
