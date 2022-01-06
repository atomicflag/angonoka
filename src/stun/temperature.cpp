#include "temperature.h"
#include <bit>
#include <boost/accumulators/statistics/stats.hpp>
#include <gsl/gsl-lite.hpp>

#ifndef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INT(x) base_value(x)
#else // NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INT(x) x
#endif // NDEBUG

namespace {
using namespace angonoka::stun;

constexpr auto initial_beta = 1.0F;

constexpr auto operator*(StunWindow i)
{
    return static_cast<std::underlying_type_t<StunWindow>>(i);
}

constexpr auto operator*(RestartPeriod i)
{
    return static_cast<std::underlying_type_t<RestartPeriod>>(i);
}
} // namespace

namespace angonoka::stun {
Temperature::Temperature(
    BetaScale beta_scale,
    StunWindow stun_window,
    RestartPeriod restart_period)
    : value{initial_beta}
    , stun_window{*stun_window}
    , acc{tag::rolling_window::window_size = *stun_window}
    , beta_scale{beta_scale}
    , restart_period_mask{*restart_period - 1}
{
    Expects(beta_scale > 0.F);
    Expects(*stun_window > 0);
    Expects(*restart_period > 0);
    Expects(std::has_single_bit(*restart_period));
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
    Expects(stun_window > 0);

    value = initial_beta;
    acc = decltype(acc){
        tag::rolling_window::window_size = INT(stun_window)};

    Ensures(value == 1.0F);
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

#undef INT
