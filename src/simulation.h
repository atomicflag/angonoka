#pragma once

#include "configuration.h"
#include "predict.h"
#include "stun/random_utils.h"
#include <chrono>
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::detail {
/**
    Simulation of agents doing tasks according to the schedule.
*/
class Simulation {
public:
    /**
      Simulation parameters.

      @var config An instance of Configuration
      @var random An instance of RandomUtils
    */
    struct Params {
        gsl::not_null<const Configuration*> config;
        gsl::not_null<stun::RandomUtils*> random;
    };

    /**
        Constructor.

        TODO: test

        @param params Simulation parameters
    */
    Simulation(const Params& params);

    /**
        Run the simulation w.r.t. the schedule.

        TODO: we don't need the whole OptimizedSchedule
        just the .schedule member variable. Refactor
        TODO: test, expects

        @param schedule An instance of OptimizedSchedule

        @return Possible makespan
    */
    [[nodiscard]] std::chrono::seconds
    operator()(const OptimizedSchedule& schedule) noexcept;

    /**
        Get current parameters.

        TODO: test

        @return Parameters.
    */
    [[nodiscard]] Params params() const;

    /**
        Set parameters.

        TODO: test

        @param params Parameters
    */
    void params(const Params& params);

    // TODO: rule of 5

private:
    struct Impl;
    gsl::not_null<const Configuration*> config;
    gsl::not_null<stun::RandomUtils*> random;
    std::vector<float> buffer;
    ranges::span<float> agent_performance;
    ranges::span<float> task_duration;
    ranges::span<float> agent_work_end;
    ranges::span<float> task_done;
};
} // namespace angonoka::detail

namespace angonoka {
// boost::histogram histogram(
//   const Configuration& config,
//   const OptimizedSchedule& schedule);
} // namespace angonoka
