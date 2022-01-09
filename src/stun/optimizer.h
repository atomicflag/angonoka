#pragma once

#include "exp_curve_fitter.h"
#include "optimizer_job.h"
#include "random_utils.h"
#include "schedule_params.h"
#include <gsl/gsl-lite.hpp>
#include <vector>

namespace angonoka::stun {
/**
    Optimization algorithm based on stochastic tunneling.

    This is the primary facade for doing stochastic tunneling
    optimization.
*/
class Optimizer {
public:
    /**
        Optimizer options.

        @var params         An instance of schedule parameters
        @var batch_size     Number of STUN iterations in each update
        @var max_idle_iters Halting condition
        @var beta_scale     Temperature parameter's inertia
        @var stun_window    Temperature adjustment window
        @var gamma          Domain-specific parameter for STUN
        @var restart_period Temperature volatility period
    */
    struct Options {
        gsl::not_null<const ScheduleParams*> params;
        int32 batch_size;
        int32 max_idle_iters;
        float beta_scale;
        int32 stun_window;
        float gamma;
        int32 restart_period;
    };

    /**
        Constructor.

        @param options Optimizer tunables
    */
    explicit Optimizer(const Options& options);

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

    // TODO: doc, test, expects
    [[nodiscard]] int32 current_epoch() const noexcept;

private:
    struct Impl;

    int32 batch_size;
    int32 max_idle_iters;
    int32 idle_iters{0};
    int32 epochs{0};
    float last_progress{0.F};
    float last_makespan{0.F};
    ExpCurveFitter exp_curve;

    /**
        An optimization job and a PRNG.

        @var random_utils   Random number generator utilities
        @var job            Optimization job
    */
    struct JobSlot {
        /**
            Constructor.

            @params options Job options
        */
        explicit JobSlot(const Options& options);

        RandomUtils random_utils;
        OptimizerJob job;
    };
    std::vector<JobSlot> jobs;
};
} // namespace angonoka::stun
