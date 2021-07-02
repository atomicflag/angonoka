#pragma once

#include "stun/schedule.h"

namespace angonoka::stun {
enum class BatchSize : std::int_fast32_t;
enum class MaxIdleIters : std::int_fast32_t;
struct Optimizer {
    Optimizer(auto...) { }

    bool has_converged();
    void update();
    Schedule schedule();
    float estimated_progress();
    float normalized_makespan();

    int steps{0};
};
} // namespace angonoka::stun
