#pragma once

#include "common.h"
#include "detail.h"
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
    Temperature parameter.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.
*/
struct Beta : detail::OpaqueFloat {
};

/**
    Scaling factor for beta parameter.

    The lower the value the slower the beta parameter
    will adjust to new stun values.
*/
struct BetaScale : detail::OpaqueFloat {
};

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

        Note: restart_period must be a power of 2.

        @param beta             Initial beta (temperature) value
        @param beta_scale       Scaling factor
        @param stun_window      STUN rolling mean window
        @param restart_period   Number of iterations before restarting
    */
    Temperature(
        Beta beta,
        BetaScale beta_scale,
        StunWindow stun_window,
        RestartPeriod restart_period);

    Temperature(const Temperature& other) = default;
    Temperature(Temperature&& other) noexcept = default;
    Temperature& operator=(const Temperature& other) = default;
    Temperature& operator=(Temperature&& other) noexcept;
    ~Temperature() = default;

    /**
        Updates the internal counters, averages and the beta value.

        The dampening parameter reduces the rate of change of
        the beta value, where 0 means no dampening and 1
        reduces the rate to 0.

        @param stun         Current STUN value
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

private:
    float value;
    accumulator_set<float, stats<tag::rolling_mean, tag::count>> acc;
    float beta_scale;
    std::size_t restart_period_mask;
};

#ifdef UNIT_TEST
struct TemperatureStub {
    virtual operator float() noexcept = 0;
    virtual void update(float stun) noexcept = 0;
    [[nodiscard]] virtual float average_stun() const noexcept = 0;
    virtual ~TemperatureStub() noexcept = default;
};
#endif // UNIT_TEST
} // namespace angonoka::stun
