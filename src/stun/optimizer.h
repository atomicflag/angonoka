#pragma once

#include "exp_curve_fitter.h"
#include "schedule_params.h"
#include <gsl/gsl-lite.hpp>
#include "optimizer_job.h"
#include <vector>

namespace angonoka::stun {
/**
    How many iterations without improvement before
    considering optimization complete.
*/
enum class MaxIdleIters : std::int_fast32_t;

/**
    Optimization algorithm based on stochastic tunneling.

    This is the primary facade for doing stochastic tunneling
    optimization.
*/
class Optimizer {
public:
    /**
        Constructor.

        @param params           Scheduling parameters
        @param batch_size       Number of iterations per update
        @param max_idle_iters   Stopping condition
    */
    Optimizer(
        const ScheduleParams& params,
        BatchSize batch_size,
        MaxIdleIters max_idle_iters);

    /**
        Run stochastic tunneling optimization batch.

        Does batch_size number of iterations and adjusts the estimated
        progress accordingly.
    */
    void update() noexcept;

    /**
        Checks if the stopping condition has been met.

        @return True when further improvements are unlikely
    */
    [[nodiscard]] bool has_converged() const noexcept;

    /**
        Estimated optimization progress from 0.0 to 1.0.

        @return Progress from 0.0 to 1.0
    */
    [[nodiscard]] float estimated_progress() const noexcept;

    /**
        The best schedule so far.

        @return A schedule.
    */
    [[nodiscard]] Schedule schedule() const;

    /**
        The best makespan so far.

        @return Makespan.
    */
    [[nodiscard]] float normalized_makespan() const;

    /**
        Reset the optimization to initial state.
    */
    void reset();

    /**
        Get the current ScheduleParams object.

        @return Schedule parameters.
    */
    [[nodiscard]] const ScheduleParams& params() const;

    /**
        Set the ScheduleParams object.

        @param params ScheduleParams object
    */
    void params(const ScheduleParams& params);

    // TODO: probably not needed
    // Optimizer(const Optimizer& other);
    // Optimizer(Optimizer&& other) noexcept;
    // Optimizer& operator=(const Optimizer& other);
    // Optimizer& operator=(Optimizer&& other) noexcept;
    // ~Optimizer() noexcept;

private:
    struct Impl;

    int16 batch_size;
    int16 max_idle_iters;
    int16 idle_iters{0};
    int16 epochs{0};
    float last_progress{0.F};
    float last_makespan{0.F};
    ExpCurveFitter exp_curve;
    int16 best_job{0};
    std::vector<OptimizerJob> jobs;
};
} // namespace angonoka::stun
