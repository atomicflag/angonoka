#include "temperature.h"
#include <bit>
#include <boost/accumulators/statistics/stats.hpp>
#include <gsl/gsl-lite.hpp>

namespace {
constexpr auto initial_beta = 1.0F;
} // namespace

namespace angonoka::stun {
Temperature::Temperature(
    BetaScale beta_scale,
    StunWindow stun_window,
    RestartPeriod restart_period)
    : value{initial_beta}
    , stun_window{static_cast<std::int_fast32_t>(stun_window)}
    , acc{tag::rolling_window::window_size
          = static_cast<std::int_fast32_t>(stun_window)}
    , beta_scale{beta_scale}
    , restart_period_mask{
          static_cast<std::size_t>(restart_period) - 1}
{
    Expects(beta_scale > 0.F);
    Expects(static_cast<std::int_fast32_t>(stun_window) > 0);
    Expects(static_cast<std::size_t>(restart_period) > 0);
    Expects(
        std::popcount(static_cast<std::size_t>(restart_period)) == 1);
    // TODO: should restart_period be greater than stun_window?
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

void Temperature::reset()
{
    value = initial_beta;
    acc = decltype(acc){
        tag::rolling_window::window_size
        = static_cast<std::int_fast32_t>(stun_window)};
}

Temperature& Temperature::operator=(Temperature&& other) noexcept
{
    value = other.value;
    stun_window = other.stun_window;
    try {
        acc = other.acc;
    } catch (...) {
        // 99% certain that the missing noexcept on
        // accumulator's copy ctor is a bug in
        // a boost lib.
#ifdef __llvm__
        __builtin_unreachable();
#endif // __llvm__
    }
    beta_scale = other.beta_scale;
    restart_period_mask = other.restart_period_mask;
    return *this;
}

Temperature::Temperature(const Temperature& other) = default;
Temperature::Temperature(Temperature&& other) noexcept = default;
Temperature& Temperature::operator=(const Temperature& other)
    = default;
Temperature::~Temperature() noexcept = default;
} // namespace angonoka::stun
