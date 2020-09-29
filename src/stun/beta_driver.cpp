#include "beta_driver.h"
#include <gsl/gsl-lite.hpp>

#ifndef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(v) static_cast<float>(base_value(v))
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(v) static_cast<float>(v)
#endif

namespace angonoka::stun {
constexpr uint32 average_stun_window = max_iterations / 100;

BetaDriver::BetaDriver(float beta, float beta_scale)
    : value{beta}
    , beta_scale{beta_scale}
{
    Expects(beta >= 0.F);
}

void BetaDriver::update(float stun, uint64 iteration) noexcept
{
    Expects(stun >= 0.F);
    Expects(iteration <= max_iterations);

    average_stun += stun;
    if (++stun_count != average_stun_window) return;
    average_stun /= TO_FLOAT(stun_count);
    last_average = average_stun;
    const auto diff = average_stun - 0.03F;
    const auto t
        = 1.F - TO_FLOAT(iteration) / TO_FLOAT(max_iterations);
    value *= 1.F + diff * beta_scale * t * t;
    stun_count = 0U;

    Ensures(value >= 0.F);
}
} // namespace angonoka::stun

#undef TO_FLOAT
