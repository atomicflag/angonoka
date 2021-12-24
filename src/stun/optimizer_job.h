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
    A single optimization job, meant to be launched in a thread pool.

    Optimizer starts many OptimizerJobs in parallel,
    pruning unseccessful jobs as needed.
*/
class OptimizerJob {
public:
    /**
        OptimizerJob options.

        TODO: doc

        @var params Schedule parameters
        @var random Random utils.
    */
    struct Options {
        gsl::not_null<const ScheduleParams*> params;
        gsl::not_null<RandomUtils*> random;
        int32 batch_size;
        float beta_scale;
        int32 stun_window;
        float gamma;
        int32 restart_period;
    };

    /**
        OptimizerJob parameters.

        @var params Schedule parameters
        @var random Random utils.
    */
    struct Params {
        gsl::not_null<const ScheduleParams*> params;
        gsl::not_null<RandomUtils*> random;
    };

    /**
        Constructor.

        TODO: expects

        @param options Job tunables
    */
    explicit OptimizerJob(const Options& options);

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
        Get current parameters.

        TODO: expects

        @return Parameters.
    */
    [[nodiscard]] Params params() const;

    /**
        Set parameters.

        TODO: expects

        @param params Parameters.
    */
    void params(const Params& params);

    OptimizerJob(const OptimizerJob& other);
    OptimizerJob(OptimizerJob&& other) noexcept;
    OptimizerJob& operator=(const OptimizerJob& other);
    OptimizerJob& operator=(OptimizerJob&& other) noexcept;
    ~OptimizerJob() noexcept;

private:
    int32 batch_size;
    Mutator mutator;
    Makespan makespan;
    Temperature temperature;
    StochasticTunneling stun;
};
} // namespace angonoka::stun
