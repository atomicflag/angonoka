#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun {
using ranges::span;

class TaskDurationCache;

/**
    Estimates makespan of a schedule.
*/
class MakespanEstimator {
public:
    /**
        Constructor.

        @param agent_count          Total number of agents
        @param task_duration_cache  Cache of task durations
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
    std::vector<float> makespan_buffer;
    gsl::not_null<const TaskDurationCache*> task_duration_cache;
};

#ifdef UNIT_TEST
struct MakespanEstimatorStub {
    virtual float operator()(span<const int16> state) noexcept = 0;
    virtual ~MakespanEstimatorStub() noexcept = default;
};
#endif // UNIT_TEST
} // namespace angonoka::stun
