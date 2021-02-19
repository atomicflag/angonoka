#include "temperature.h"
#include <bit>
#include <boost/accumulators/statistics/stats.hpp>
#include <gsl/gsl-lite.hpp>

namespace angonoka::stun {
Temperature::Temperature(
    Beta beta,
    BetaScale beta_scale,
    StunWindow stun_window,
    RestartPeriod restart_period)
    : value{beta}
    , acc{tag::rolling_window::window_size
          = static_cast<std::int_fast32_t>(stun_window)}
    , beta_scale{beta_scale}
    , restart_period_mask{
          static_cast<std::size_t>(restart_period) - 1}
{
    Expects(beta > 0.F);
    Expects(beta_scale > 0.F);
    Expects(static_cast<std::int_fast32_t>(stun_window) > 0);
    Expects(static_cast<std::size_t>(restart_period) > 0);
    Expects(
        std::popcount(static_cast<std::size_t>(restart_period)) == 1);
}

[[nodiscard]] float Temperature::average_stun() const noexcept
{
    using boost::accumulators::rolling_mean;
    return rolling_mean(acc);
}

void Temperature::update(float stun) noexcept
{
    using boost::accumulators::count;
    Expects(stun >= 0.F);

    acc(stun);
    const auto dampening
        = static_cast<float>(count(acc) & restart_period_mask)
        / static_cast<float>(restart_period_mask + 1);
    const auto diff = average_stun() - 0.03F;
    const auto t = 1.F - dampening;
    value *= 1.F + diff * beta_scale * t * t;

    Ensures(value >= 0.F);
}
} // namespace angonoka::stun
