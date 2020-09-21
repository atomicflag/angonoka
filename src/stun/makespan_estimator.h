#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>

namespace angonoka::stun {
using ranges::span;

class TaskDurationCache;

/**
    Estimates makespan of a given configuration.
*/
class MakespanEstimator {
public:
    /**
        Constructor.

        @param agent_count Total number of agents
        @param task_duration_cache Cache of task durations
    */
    MakespanEstimator(
        gsl::index agent_count,
        gsl::not_null<const TaskDurationCache*>
            task_duration_cache) noexcept;

    /**
        Estimates the highest total duration of all tasks
        for any given agent.

        @param state Agent assignments

        @return Makespan in seconds
    */
    float operator()(span<const int16> state) noexcept;

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    std::unique_ptr<float[]> makespan_buffer_data;
    span<float> makespan_buffer;
    gsl::not_null<const TaskDurationCache*> task_duration_cache;
};
} // namespace angonoka::stun
