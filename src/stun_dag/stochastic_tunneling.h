#pragma once

#include "beta_driver.h"
#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun_dag {

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
    TODO: doc
    Result of a stochastic tunneling pass.

    @var energy Lowest energy achieved so far
    @var state  State that had the lowest energy
    @var beta   Final temperature
*/
struct STUNResult {
    // std::vector<int16> state;
    float energy;
    float beta;
};

struct STUNOptions {
    gsl::no_null<const ScheduleInfo*> info;
    gsl::not_null<RandomUtils*> random;
    gsl::not_null<Makespan*> makespan;
    gsl::not_null<Temperature*> temp;
};

/**
    TODO: doc
*/
STUNResult stochastic_tunneling(
    STUNOptions options,
    MutState state) noexcept;

} // namespace angonoka::stun_dag
