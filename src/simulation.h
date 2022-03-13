#pragma once

#include "configuration.h"
#include "predict.h"
#include "stun/random_utils.h"
#include "stun/schedule.h"
#include <boost/histogram.hpp>
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
using Histogram = boost::histogram::histogram<
    std::tuple<boost::histogram::axis::regular<
        float,
        boost::use_default,
        boost::histogram::axis::null_type,
        boost::histogram::axis::option::growth_t>>>;

// TODO: doc, test, expects
Histogram histogram(
    const Configuration& config,
    const OptimizedSchedule& schedule);
} // namespace angonoka
