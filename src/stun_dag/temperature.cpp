#include "temperature.h"
#include <gsl/gsl-lite.hpp>

#ifndef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(v) static_cast<float>(base_value(v))
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(v) static_cast<float>(v)
#endif

namespace angonoka::stun_dag {
constexpr auto stun_window_count = 100;

Temperature::Temperature(
    Beta beta,
    BetaScale beta_scale,
    StunWindow stun_window)
    : value{beta}
    , stun_window{static_cast<std::int_fast32_t>(stun_window_count)}
    , beta_scale{beta_scale}
{
    Expects(beta > 0.F);
    Expects(beta_scale > 0.F);
    Expects(static_cast<std::int_fast32_t>(stun_window) > 0);
}

void Temperature::update(float stun, float dampening) noexcept
{
    Expects(stun >= 0.F);
    Expects(dampening >= 0.F);

    average_stun += stun;
    if (++stun_count < stun_window) return;
    average_stun /= TO_FLOAT(stun_count);
    last_average = average_stun;
    const auto diff = average_stun - 0.03F;
    const auto t = 1.F - dampening;
    value *= 1.F + diff * beta_scale * t * t;
    stun_count = 0U;

    Ensures(value >= 0.F);
}
} // namespace angonoka::stun_dag

#undef TO_FLOAT
