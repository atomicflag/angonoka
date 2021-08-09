#pragma once

#include "random_utils.h"
#include "schedule.h"
#include "schedule_params.h"
#include "stochastic_tunneling.h"
#include "temperature.h"
#include "utils.h"
#include <gsl/gsl-lite.hpp>

namespace angonoka::stun {
/**
    How many stochastic tunneling iterations to go through
    during each update.
*/
enum class BatchSize : std::int_fast32_t;

/**
    A single optimization job, meant to be launched in a thread pool.

    Optimizer starts many OptimizerJobs in parallel,
    pruning unseccessful jobs as needed.
*/
class OptimizerJob {
public:
    /**
        OptimizerJob options.

        @var params Schedule parameters
        @var random Random utils.
    */
    struct Options {
        gsl::not_null<const ScheduleParams*> params;
        gsl::not_null<RandomUtils*> random;
    };

    /**
        Constructor.

        @param params           Scheduling parameters
        @param random_utils     Random number generator utilities
        @param batch_size       Number of iterations per update
    */
    OptimizerJob(
        const ScheduleParams& params,
        RandomUtils& random_utils,
        BatchSize batch_size);
    OptimizerJob(const Options& options, BatchSize batch_size);

    /**
        Run stochastic tunneling optimization batch.

        Does batch_size number of iterations.
    */
    void update() noexcept;

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
        Get current options.

        @return Options.
    */
    [[nodiscard]] Options options() const;

    /**
        Set options.

        @param options Options.
    */
    void options(const Options& options);

    OptimizerJob(const OptimizerJob& other);
    OptimizerJob(OptimizerJob&& other) noexcept;
    OptimizerJob& operator=(const OptimizerJob& other);
    OptimizerJob& operator=(OptimizerJob&& other) noexcept;
    ~OptimizerJob() noexcept;

private:
    static constexpr auto beta_scale = 1e-4F;
    static constexpr auto stun_window = 10000;
    static constexpr auto gamma = .5F;
    static constexpr auto restart_period = 1 << 20;
    static constexpr auto initial_beta = 1.0F;

    int16 batch_size;
    Mutator mutator;
    Makespan makespan;
    Temperature temperature;
    StochasticTunneling stun;
};
} // namespace angonoka::stun