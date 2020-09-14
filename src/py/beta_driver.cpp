#include "beta_driver.h"
#include <gsl/gsl-lite.hpp>

namespace angonoka::stun {
constexpr std::uint_fast32_t average_stun_window
    = max_iterations / 100;

BetaDriver::BetaDriver(float beta, float beta_scale)
    : value{beta}
    , beta_scale{beta_scale}
{
    Expects(beta >= 0.F);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BetaDriver::update(
    float stun,
    std::uint_fast64_t iteration) noexcept
{
    Expects(stun >= 0.F);
    Expects(iteration <= max_iterations);

    average_stun += stun;
    if (++stun_count != average_stun_window) return;
    average_stun /= static_cast<float>(stun_count);
    last_average = average_stun;
    const auto diff = average_stun - 0.03F;
    const auto t
        = 1.F - static_cast<float>(iteration) / max_iterations;
    value *= 1.F + diff * beta_scale * t * t;
    stun_count = 0U;
}
} // namespace angonoka::stun
