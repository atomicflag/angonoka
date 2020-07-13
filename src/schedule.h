#pragma once

#include "system.h"
#include <array>
#include <boost/dynamic_bitset.hpp>
#include <gsl/gsl-lite.hpp>
#include <pcg_random.hpp>
#include <range/v3/view/span.hpp>

namespace angonoka::detail {
using IndividualView = gsl::span<const std::int_fast8_t>;
using Individual = gsl::span<std::int_fast8_t>;
using ExpectedDurations
    = Vector<std::int_fast32_t, static_alloc_tasks>;
using ExpectedPerformance = Vector<float, static_alloc_agents>;
using AgentGroups = boost::dynamic_bitset<>;
using Parents = std::array<IndividualView, 3>;
using RandomEngine = pcg32;

/**
    Pre-calculated system constraints.

    Used for solving the optimal schedule problem.

    @var durations      Expected (average) task durations in seconds.
    @var performance    Expected (average) agent performances.
    @var agent_groups   Pre-calculated task availabilities.
*/
struct Constraints {
    ExpectedDurations durations;
    ExpectedPerformance performance;
    AgentGroups agent_groups;

    /**
        Calculate constraints based on the information in the System.

        @param sys System instance.
    */
    explicit Constraints(const System& sys);

    /**
        Check if an agent can perform a task.

        @param agent_id Agent's index.
        @param task_id  Task's index.

        @return True if the agent can perform the task.
    */
    [[nodiscard]] bool can_work_on(
        std::int_fast8_t agent_id,
        std::int_fast8_t task_id) const;
};

/**
    Computes the makespan of the genetic algorithm's individual.

    @param i    GA Individual
    @param con  System constraints
    @param buf  Buffer to hold the accumulated results

    @returns Makespan in seconds.
*/
std::int_fast32_t makespan(
    IndividualView i,
    const Constraints& con,
    gsl::span<std::int_fast32_t> buf);

/**
    Performs a GA crossover operation.

    Creates a new individual i by mixing genetic code from
    parents p.

    @param p    An array of parents
    @param i    Child individual
    @param gen  Pseudorandom number generator
*/
void crossover(Parents p, Individual i, RandomEngine& gen);
} // namespace angonoka::detail

namespace angonoka {
} // namespace angonoka
