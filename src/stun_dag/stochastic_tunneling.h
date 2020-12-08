#pragma once

#include "common.h"
#include "detail.h"
#include <gsl/gsl-lite.hpp>
#include <vector>

namespace angonoka::stun_dag {

class Temperature;
class Makespan;
class RandomUtils;
struct ScheduleInfo;

/**
    Tunneling parameter.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.
*/
struct Gamma : detail::OpaqueFloat {
};

/**
    Result of a stochastic tunneling pass.

    @var state  State that had the lowest energy
    @var energy Lowest energy achieved so far
    @var beta   Final temperature
*/
struct STUNResult {
    std::vector<StateItem> state;
    float energy;
    float beta;
};

/**
    // TODO: mocks

    STUN auxilary data and utilities.

    @var info       Instance of ScheduleInfo
    @var random     Instance of RandomUtils
    @var makespan   Instance of Makespan
    @var temp       Instance of Temperature
*/
struct STUNOptions {
    gsl::not_null<const ScheduleInfo*> info;
    gsl::not_null<RandomUtils*> random;
    gsl::not_null<Makespan*> makespan;
    gsl::not_null<Temperature*> temp;
};

/**
    Perform a stochastic tunneling pass.

    See https://arxiv.org/pdf/physics/9903008.pdf for more details.

    @param state        Initial state
    @param STUNOptions  Data and utilities

    @return An instance of STUNResult
*/
STUNResult
stochastic_tunneling(MutState state, STUNOptions options) noexcept;

} // namespace angonoka::stun_dag
