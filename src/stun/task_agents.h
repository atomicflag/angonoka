#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

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
    TaskAgents(span<const span<const int16>> data);

    /**
        Retrieves the array of agents that can perform a given task.

        @param i Task index

        @return An array of agent ids that can perform this task
    */
    decltype(auto) operator[](index i) const noexcept
    {
        Expects(i >= 0);
        Expects(static_cast<gsl::index>(i) < task_agents.size());

        return task_agents[static_cast<gsl::index>(i)];
    }

    TaskAgents(const TaskAgents& other);
    TaskAgents(TaskAgents&&) = default;
    TaskAgents& operator=(const TaskAgents& other);
    TaskAgents& operator=(TaskAgents&&) = default;
    ~TaskAgents() = default;

private:
    std::vector<int16> int_data;
    std::vector<span<const int16>> task_agents;
};

#ifdef UNIT_TEST
struct TaskAgentsStub {
    virtual span<const int16> operator[](index i) const noexcept = 0;
    virtual ~TaskAgentsStub() noexcept = default;
};
#endif // UNIT_TEST
} // namespace angonoka::stun
