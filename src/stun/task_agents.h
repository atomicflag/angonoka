#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>

// TODO: Tests

namespace angonoka::stun {
using ranges::span;

/**
    Holds the array of agent ids for each task.

    Answers the question "which agents can perform this task?".
*/
class TaskAgents {
public:
    /**
        Default constructor.
    */
    TaskAgents() = default;

    /**
        Constructor.

        @param data Array of arrays of agent ids for each task
    */
    TaskAgents(span<span<const int16>> data);

    /**
        Retrieves the array of agents that can perform a given task.

        @param i Task index

        @return An array of agent ids that can perform this task
    */
    decltype(auto) operator[](index i) const noexcept
    {
        Expects(i >= 0);
        Expects(i < task_agents.size());

        return task_agents[i];
    }

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    std::unique_ptr<int16[]> int_data;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    std::unique_ptr<span<const int16>[]> spans;
    span<span<const int16>> task_agents;
};
} // namespace angonoka::stun
