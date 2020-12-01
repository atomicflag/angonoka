#include "temperature.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <gsl/gsl-lite.hpp>

namespace angonoka::stun_dag {
Temperature::Temperature(
    Beta beta,
    BetaScale beta_scale,
    StunWindow stun_window)
    : value{beta}
    , acc{tag::rolling_window::window_size
          = static_cast<std::int_fast32_t>(stun_window)}
    , beta_scale{beta_scale}
{
    Expects(beta > 0.F);
    Expects(beta_scale > 0.F);
    Expects(static_cast<std::int_fast32_t>(stun_window) > 0);
}

[[nodiscard]] float Temperature::average_stun() const noexcept
{
    using boost::accumulators::rolling_mean;
    return rolling_mean(acc);
}

void Temperature::update(float stun, float dampening) noexcept
{
    using boost::accumulators::rolling_mean;
    Expects(stun >= 0.F);
    Expects(dampening >= 0.F);

    acc(stun);
    const auto diff = average_stun() - 0.03F;
    const auto t = 1.F - dampening;
    value *= 1.F + diff * beta_scale * t * t;

    Ensures(value >= 0.F);
}
} // namespace angonoka::stun_dag
