#pragma once

#include "common.h"
#include <cstdint>

namespace angonoka::stun {
class BetaDriver {
public:
    BetaDriver(float beta, float beta_scale);

    void update(float stun, std::uint_fast64_t iteration) noexcept;
    [[nodiscard]] float beta() const noexcept;
    [[nodiscard]] float last_average_stun() const noexcept;

private:
    float value;
    float average_stun{.0F};
    float last_average{.0F};
    std::uint_fast32_t stun_count{0};

    float beta_scale; // TODO: Temporary, should be hardcoded
};
} // namespace angonoka::stun
