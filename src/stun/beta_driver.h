#pragma once

#include "common.h"
#include <cstdint>

namespace angonoka::stun {
/**
    Updates beta (temperature) value to keep the
    average STUN value around 0.03 as recommended
    in the paper

    https://arxiv.org/pdf/physics/9903008.pdf
*/
class BetaDriver {
public:
    /**
        Constructor.

        @param beta Initial beta (temperature) value
    */
    BetaDriver(
        float beta,
        float beta_scale); // TODO: beta_scale should be hardcoded

    /**
        Updates the internal counters, averages and the beta value.

        @param stun         Current STUN value
        @param iteration    Current iteration number
    */
    void update(float stun, uint64 iteration) noexcept;

    /**
        Returns the current beta (temperature) value.

        @retun Beta value
    */
    [[nodiscard]] float beta() const noexcept { return value; }

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
    uint32 stun_count{0};

    float beta_scale; // TODO: Temporary, should be hardcoded
};
} // namespace angonoka::stun
