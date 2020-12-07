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
    TODO: doc
    Result of a stochastic tunneling pass.

    @var energy Lowest energy achieved so far
    @var state  State that had the lowest energy
    @var beta   Final temperature
*/
struct STUNResult {
    std::vector<StateItem> state;
    float energy;
    float beta;
};

struct STUNOptions {
    gsl::not_null<const ScheduleInfo*> info;
    gsl::not_null<RandomUtils*> random;
    gsl::not_null<Makespan*> makespan;
    gsl::not_null<Temperature*> temp;
};

/**
    TODO: doc
*/
STUNResult
stochastic_tunneling(STUNOptions options, MutState state) noexcept;

} // namespace angonoka::stun_dag
