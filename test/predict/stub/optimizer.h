#pragma once

#include "stun/schedule.h"

namespace angonoka::stun {
struct ScheduleParams;
struct Optimizer {
    struct Options {
        const ScheduleParams* params;
        int batch_size;
        int max_idle_iters;
        float beta_scale;
        int stun_window;
        float gamma;
        int restart_period;
    };
    Optimizer(const Options&) { }

    bool has_converged();
    void update();
    Schedule schedule();
    float estimated_progress();
    float normalized_makespan() const;

    int steps{0};
};
} // namespace angonoka::stun
