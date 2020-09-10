#pragma once

#include <gsl/gsl-lite.hpp>
#include "makespan_estimator.h"
#include <range/v3/view/span.hpp>
#include <memory>
#include "common.h"
#include "beta_driver.h"

namespace angonoka::stun {
    using ranges::span;

    class RandomUtils;

// TODO: Can we use boost safe_numerics for this?
struct OpaqueFloat {
    float value;
    operator float() { return value; }
};

struct Alpha : OpaqueFloat {
};
struct Beta : OpaqueFloat {
};
struct BetaScale : OpaqueFloat {
};

class StochasticTunneling {
public:
    StochasticTunneling(
        gsl::not_null<RandomUtils*> random_utils,
        MakespanEstimator&& makespan,
        span<const int16> best_state,
        Alpha alpha,
        Beta beta,
        BetaScale beta_scale);

    void run() noexcept;
    float lowest_e() const noexcept;
    span<const int16> best_state() const noexcept;
    float beta() const noexcept;

private:
    using Counter = std::uint_fast64_t;

    gsl::not_null<RandomUtils*> random_utils;
    MakespanEstimator makespan;

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
