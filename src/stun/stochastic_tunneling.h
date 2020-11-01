#pragma once

#include "beta_driver.h"
#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun {

namespace detail {
    /**
        Opaque floating point type.
    */
    struct OpaqueFloat {
        float value;
        operator float() const noexcept { return value; }
    };
} // namespace detail

using ranges::span;

#ifndef UNIT_TEST
using RandomUtilsT = class RandomUtils;
using MakespanEstimatorT = class MakespanEstimator;
#else // UNIT_TEST
using RandomUtilsT = struct RandomUtilsStub;
using MakespanEstimatorT = struct MakespanEstimatorStub;
#endif // UNIT_TEST

/**
    Tunneling parameter.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.
*/
struct Gamma : detail::OpaqueFloat {
};

/**
    Temperature parameter.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.

    Note: Unlike the beta parameter in the paper, this isn't
    an inverse temperature.
*/
struct Beta : detail::OpaqueFloat {
};

// TODO: Temporary, should be hard-coded
struct BetaScale : detail::OpaqueFloat {
};

/**
    Result of a stochastic tunneling pass.

    @var energy Lowest energy achieved so far
    @var state  State that had the lowest energy
    @var beta   Final temperature
*/
struct STUNResult {
    float energy;
    std::vector<int16> state;
    float beta;
};

/**
    Perform a stochastic tunneling pass.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.

    @param random_utils An instance of RandomUtils
    @param makespan     An instance of Makespan
    @param state        Initial state
    @param gamma        Tunneling parameter
    @param beta         Initial temperature

    @return An instance of STUNResult
*/
STUNResult stochastic_tunneling(
    RandomUtilsT& random_utils,
    MakespanEstimatorT& makespan,
    span<const int16> state,
    Gamma gamma,
    Beta beta,
    // TODO: Temporary, should be hardcoded
    BetaScale beta_scale);

} // namespace angonoka::stun
