#pragma once

#include "schedule.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun {
using ranges::span;
struct ScheduleParams;

/**
    Makespan estimator.

    Further reading:
    https://en.wikipedia.org/wiki/Makespan
*/
class Makespan {
public:
    /**
        Constructor.

        @param params       An instance of ScheduleParams
        @param tasks_count  Total number of tasks
        @param agents_count Total number of agents
    */
    Makespan(const ScheduleParams& params);

    Makespan(const Makespan& other);
    Makespan& operator=(const Makespan& other) noexcept;
    Makespan(Makespan&& other) noexcept;
    Makespan& operator=(Makespan&& other) noexcept;
    ~Makespan() noexcept;

    /**
        Calculate the makespan of a given scheduling configuration.

        @param schedule Scheduling configuration

        @return Makespan in seconds
    */
    float operator()(Schedule schedule) noexcept;

private:
    struct Impl;
    gsl::not_null<const ScheduleParams*> params;
    std::vector<float> sum_buffer;
    span<float> task_done;
    span<float> work_done;
};

class RandomUtils;

/**
    Shuffle tasks and agents in-place.

    Randomly swaps two adjacent tasks within the schedule and
    reassigns an agent of a random task.
*/
class Mutator {
public:
    /**
        Constructor.

        @param params An instance of ScheduleParams
        @param random An instance of RandomUtils
    */
    Mutator(const ScheduleParams& params, RandomUtils& random);

    /**
        Mutates the scheduling configuration in-place.

        @param schedule Scheduling configuration
    */
    void operator()(MutSchedule schedule) const noexcept;

private:
    struct Impl;
    gsl::not_null<const ScheduleParams*> params;
    gsl::not_null<RandomUtils*> random;
};
} // namespace angonoka::stun
