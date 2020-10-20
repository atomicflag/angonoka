#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun {
using ranges::span;

/**
    An opaque type representing an agent's position
    within the agents array.
*/
enum class AgentIndex : index {};

/**
    An opaque type representing a task's position
    within the tasks array.
*/
enum class TaskIndex : index {};

/**
    Pre-computes all task durations for all agents.
*/
class TaskDurationCache {
public:
    /**
        Default constructor.
    */
    TaskDurationCache() = default;

    /**
        Constructor.

        task_durations should contain task durations in seconds
        for each task, and agent_performances should contain
        agent performance multipliers (typically ranging from
        0.1 to 2.0) for each agent.

        @param task_durations       An array of task durations
        @param agent_performances   An array of agent performances
    */
    TaskDurationCache(
        span<const float> task_durations,
        span<const float> agent_performances);

    /**
        Retrieves the task duration when performed by the
        specified agent.

        A simple lookup in a pre-computed 2D array.

        @param agent    Agent's index
        @param task     Task's index

        @return Task duration in seconds
    */
    [[nodiscard]] float
    get(AgentIndex agent, TaskIndex task) const noexcept;

private:
    std::vector<float> float_data;
    index agent_count;

    /**
        Find the task duration's index within the cache.

        The cache is stored as a 1D array.

        @param agent    Agent's index
        @param task     Task's index

        @return Task duration's index
    */
    [[nodiscard]] index
    build_index(AgentIndex agent, TaskIndex task) const noexcept;
};

} // namespace angonoka::stun
