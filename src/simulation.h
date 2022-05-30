#pragma once

#include "predict.h"
#include "project.h"
#include "stun/schedule.h"
#include <chrono>
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

#ifndef UNIT_TEST
#include "stun/random_utils.h"
#else // UNIT_TEST
#include "stub/random_utils.h"
#endif // UNIT_TEST

namespace angonoka::detail {
/**
    Simulation of agents doing tasks according to the schedule.
*/
class Simulation {
public:
    /**
      Simulation parameters.

      @var config An instance of Project
      @var random An instance of RandomUtils
    */
    struct Params {
        gsl::not_null<const Project*> config;
        gsl::not_null<stun::RandomUtils*> random;
    };

    /**
        Constructor.

        @param params Simulation parameters
    */
    Simulation(const Params& params);

    /**
        Run the simulation w.r.t. the schedule.

        @param schedule An instance of OptimizedSchedule

        @return Possible makespan
    */
    [[nodiscard]] std::chrono::seconds operator()(
        ranges::span<const stun::ScheduleItem> schedule) noexcept;

    /**
        Get current parameters.

        @return Parameters.
    */
    [[nodiscard]] Params params() const;

    /**
        Set parameters.

        @param params Parameters
    */
    void params(const Params& params);

    Simulation(const Simulation& other);
    Simulation(Simulation&& other) noexcept;
    Simulation& operator=(const Simulation& other);
    Simulation& operator=(Simulation&& other) noexcept;
    ~Simulation() noexcept;

private:
    struct Impl;
    gsl::not_null<const Project*> config;
    gsl::not_null<stun::RandomUtils*> random;
    std::vector<float> buffer;
    ranges::span<float> agent_performance;
    ranges::span<float> task_duration;
    ranges::span<float> agent_work_end;
    ranges::span<float> task_done;
};

/**
    Pick the histogram bin width based on the expected makespan.

    @param makespan Expected makespan

    @return Histogram bin width in seconds.
*/
[[nodiscard]] float granularity(std::chrono::seconds makespan);
} // namespace angonoka::detail

namespace angonoka {
/**
    Make a histogram of simulation runs.

    @param config   Tasks and agents
    @param schedule An optimized schedule

    @return Makespan histogram
*/
[[nodiscard]] Histogram
histogram(const Project& config, const OptimizedSchedule& schedule);

/**
    Find various percentiles of the histogram.

    Find the makespan for 25, 50, 75, 95 and 99 percent
    confidence intervals.

    @param histogram Makespan histogram

    @return A struct of percentile values.
*/
HistogramStats stats(const Histogram& histogram);
} // namespace angonoka
