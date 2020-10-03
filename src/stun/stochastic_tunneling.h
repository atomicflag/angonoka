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
//
// in .cpp:
// struct State { ... }
//
// void perform_stun(State*, ...)
class StochasticTunneling {
public:
    StochasticTunneling(
        gsl::not_null<RandomUtils*>
            random_utils, // TODO: Can we move this instead? Probably
                          // not, we need to keep the PRNG state
                          // around
        MakespanEstimator&& makespan,
        span<const int16> best_state, // TODO: Starting state?
        Alpha alpha,
        Beta beta,
        BetaScale beta_scale);

    STUNResult operator()() noexcept;

private:
    using Counter = std::uint_fast64_t;

    gsl::not_null<RandomUtils*> random_utils;
    MakespanEstimator makespan;

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,
    // modernize-avoid-c-arrays)
    std::unique_ptr<int16[]> int_data;
    span<int16> current_state;
    span<int16> target_state;
    span<int16> best_state_;

    float current_e;
    float lowest_e_;
    float target_e;

    float alpha; // TODO: Should be a constant
    float current_s;
    float target_s;
    BetaDriver beta_driver;

    std::uint_fast64_t current_iteration{0};

    void get_new_neighbor() noexcept;
    bool compare_energy_levels() noexcept;
    void perform_stun() noexcept;
};
} // namespace angonoka::stun
