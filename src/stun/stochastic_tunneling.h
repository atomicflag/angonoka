#pragma once

#include "beta_driver.h"
#include "common.h"
#include "makespan_estimator.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>

// TODO: Test, docs, Expects

namespace angonoka::stun {
using ranges::span;

class RandomUtils;

struct OpaqueFloat {
    float value;
    operator float() const { return value; }
};

struct Alpha : OpaqueFloat {
};
struct Beta : OpaqueFloat {
};
struct BetaScale : OpaqueFloat {
};

struct STUNResult {
    float lowest_e;
    span<const int16> best_state;
    float beta;
};

// TODO: Refactor StochasticTunneling
// add
//
// result_t stochastic_tunneling(args...)
//
// where result_t has all the important values
// and stochastic_tunneling constructs a class
// inside the implementation (if nessecary)
//
// 3 oct:
//
// in .h:
// STUNResult stochastic_tunneling(args...)

STUNResult stochastic_tunneling(
    RandomUtils& random_utils,
    MakespanEstimator&& makespan,
    span<const int16> best_state,
    Alpha alpha,
    Beta beta,
    BetaScale beta_scale) noexcept;

} // namespace angonoka::stun
