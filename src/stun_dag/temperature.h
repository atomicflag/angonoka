#pragma once

#include "common.h"
#include "detail.h"
#include <cstdint>

namespace angonoka::stun_dag {

struct Beta : detail::OpaqueFloat {
};
struct BetaScale : detail::OpaqueFloat {
};
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
        TODO: Doc
        Constructor.

        @param beta Initial beta (temperature) value
    */
    Temperature(
        Beta beta,
        BetaScale beta_scale,
        StunWindow stun_window);

    /**
        Updates the internal counters, averages and the beta value.
        TODO: doc

        @param stun         Current STUN value
        @param dampening
    */
    void update(float stun, float dampening) noexcept;

    /**
        Returns the current beta (temperature) value.
        TODO: doc

        @retun Beta value
    */
    operator float() const noexcept { return value; }

    /**
        Returns the last average stun value for diagnostic purposes.

        @return Average STUN value over the last period
    */
    [[nodiscard]] float last_average_stun() const noexcept
    {
        return last_average;
    }

private:
    float value;
    float average_stun{.0F};
    float last_average{.0F};
    int32 stun_count{0};
    int32 stun_window;

    float beta_scale; // TODO: Temporary, should be hardcoded
};
} // namespace angonoka::stun_dag
