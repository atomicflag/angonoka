#pragma once

#include "beta_driver.h"
#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>
#include <vector>

// TODO: Test, docs, Expects

namespace angonoka::stun {

namespace detail {
    struct OpaqueFloat {
        float value;
        operator float() const noexcept { return value; }
    };
} // namespace detail

using ranges::span;

class RandomUtils;
class MakespanEstimator;

struct Alpha : detail::OpaqueFloat {
};
struct Beta : detail::OpaqueFloat {
};
struct BetaScale : detail::OpaqueFloat {
};

struct STUNResult {
    float lowest_e;
    std::vector<int16> best_state;
    float beta;
};

STUNResult stochastic_tunneling(
    RandomUtils& random_utils,
    MakespanEstimator& makespan,
    span<const int16> best_state,
    Alpha alpha,
    Beta beta,
    BetaScale beta_scale);

} // namespace angonoka::stun
