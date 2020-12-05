#pragma once

#include "common.h"
#include "detail.h"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <cstdint>

namespace angonoka::stun_dag {
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
    Updates beta (temperature) value to keep the
    average STUN value around 0.03 as recommended
    in the paper

    https://arxiv.org/pdf/physics/9903008.pdf
*/
class Temperature {
public:
    /**
        Constructor.

        @param beta         Initial beta (temperature) value
        @param beta_scale   Scaling factor
        @param stun_window  STUN rolling mean window
    */
    Temperature(
        Beta beta,
        BetaScale beta_scale,
        StunWindow stun_window);

    /**
        Updates the internal counters, averages and the beta value.

        The dampening parameter reduces the rate of change of
        the beta value, where 0 means no dampening and 1
        reduces the rate to 0.

        @param stun         Current STUN value
        @param dampening    Dampening value
    */
    void update(float stun, float dampening) noexcept;

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
    accumulator_set<float, stats<tag::rolling_mean>> acc;
    float beta_scale;
};
} // namespace angonoka::stun_dag
