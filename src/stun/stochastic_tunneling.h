#pragma once

#include "common.h"
#include "detail.h"
#include <gsl/gsl-lite.hpp>
#include <vector>

namespace angonoka::stun {

struct ScheduleInfo;
#ifndef UNIT_TEST
using TemperatureT = class Temperature;
using MakespanT = class Makespan;
using RandomUtilsT = class RandomUtils;
using MutatorT = class Mutator;
#else // UNIT_TEST
using TemperatureT = struct TemperatureStub;
using MakespanT = struct MakespanStub;
using RandomUtilsT = struct RandomUtilsStub;
using MutatorT = struct MutatorStub;
#endif // UNIT_TEST

/**
    Result of a stochastic tunneling pass.

    @var state          State that had the lowest energy
    @var energy         Lowest energy achieved so far
    @var temperature    Final temperature
*/
struct STUNResult {
    std::vector<StateItem> state;
    float energy;
    float temperature;
};

/**
    STUN auxilary data and utilities.

    @var mutator    Instance of Mutator
    @var random     Instance of RandomUtils
    @var makespan   Instance of Makespan
    @var temp       Instance of Temperature
    @var gamma      Tunneling parameter
*/
struct STUNOptions {
    gsl::not_null<const MutatorT*> mutator;
    gsl::not_null<RandomUtilsT*> random;
    gsl::not_null<MakespanT*> makespan;
    gsl::not_null<TemperatureT*> temp;
    float gamma;
};

/**
    Perform a stochastic tunneling pass.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.

    @param state        Initial state
    @param STUNOptions  Data and utilities

    @return An instance of STUNResult
*/
STUNResult stochastic_tunneling(
    State state,
    const STUNOptions& options) noexcept;

} // namespace angonoka::stun
