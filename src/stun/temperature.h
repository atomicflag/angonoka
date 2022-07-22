#pragma once

#include "detail.h"
#include "schedule.h"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <cstdint>

namespace angonoka::stun {
using boost::accumulators::accumulator_set;
using boost::accumulators::stats;
namespace tag = boost::accumulators::tag;

/**
    Scaling factor for beta parameter.

    The lower the value the slower the beta parameter
    will adjust to new stun values.
*/
struct BetaScale : detail::OpaqueFloat { };

/**
    STUN rolling mean window.

    The lower the value the more volitile the temperature gets.
*/
enum class StunWindow : std::int_fast32_t;

/**
    Number of iterations before a restart.

    The temperature volatility starts off high and slowly drops
    to 0 with each iteration. This value controls how many iterations
    it takes for the temperature to go through the full cycle.
*/
enum class RestartPeriod : std::size_t;

/**
    Updates beta (temperature) value to keep the
    average STUN value around 0.03 as recommended
    in the paper

    https://arxiv.org/pdf/physics/9903008.pdf
*/
// NOLINTNEXTLINE: bugprone-exception-escape
class Temperature {
public:
    /**
        Constructor.

        Initial temperature is 1.0.

        Note: restart_period must be a power of 2.

        @param beta_scale       Scaling factor
        @param stun_window      STUN rolling mean window
        @param restart_period   Number of iterations before restarting
    */
    Temperature(
        BetaScale beta_scale,
        StunWindow stun_window,
        RestartPeriod restart_period);

    Temperature(const Temperature& other);
    Temperature(Temperature&& other) noexcept;
    Temperature& operator=(const Temperature& other);
    Temperature& operator=(Temperature&& other) noexcept;
    ~Temperature() noexcept;

    /**
        Updates the internal counters, averages and the beta value.

        @param stun Current STUN value
    */
    void update(float stun) noexcept;

    /**
        Returns the current beta (temperature) value.

        @retun Beta value
    */
    operator float() const noexcept { return value; }

    /**
        Returns the last average stun value for diagnostic purposes.

        @return Average STUN value over the last period
    */
    [[nodiscard]] float average_stun() const noexcept;

    /**
        Reset the temperature to the initial value of 1.0.
    */
    void reset();

private:
    float value;
    int32 stun_window;
    accumulator_set<float, stats<tag::rolling_mean, tag::count>> acc;
    float beta_scale;
    std::size_t restart_period_mask;
};
} // namespace angonoka::stun
