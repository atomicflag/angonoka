#pragma once

#include "stun/common.h"

namespace angonoka::stun {
enum class BatchSize : std::int_fast32_t;
enum class MaxIdleIters : std::int_fast32_t;
struct Optimizer {
    Optimizer(auto...) { }

    bool has_converged();
    void update();
    State state();
    float estimated_progress();

    int steps{0};
};
} // namespace angonoka::stun
