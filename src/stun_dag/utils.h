#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun_dag {
using ranges::span;
struct ScheduleInfo;

/**
    Makespan estimator.

    Further reading:
    https://en.wikipedia.org/wiki/Makespan
*/
class Makespan {
public:
    /**
        Constructor.

        @param info         An instance of ScheduleInfo
        @param tasks_count  Total number of tasks
        @param agents_count Total number of agents
    */
    Makespan(const ScheduleInfo& info);

    Makespan(const Makespan& other);
    Makespan& operator=(const Makespan& other) noexcept;
    Makespan(Makespan&& other) noexcept;
    Makespan& operator=(Makespan&& other) noexcept;
    ~Makespan() noexcept;

    /**
        Calculate the makespan of a given scheduling configuration.

        @param state Scheduling configuration

        @return Makespan in seconds
    */
    float operator()(State state) noexcept;

private:
    gsl::not_null<const ScheduleInfo*> info;
    std::vector<float> sum_buffer;
    span<float> task_done;
    span<float> work_done;

    /**
        The time when the last dependency of a given task will be
        completed.

        @param task_id Task's index

        @return Time in seconds
    */
    [[nodiscard]] float
    dependencies_done(int16 task_id) const noexcept;

    /**
        How long it will take for a given agent to complete a given
        task.

        Factors in agent's performace.

        @param task_id Task's index
        @param agent_id Agent's index

        @return Time in seconds
    */
    [[nodiscard]] float
    task_duration(int16 task_id, int16 agent_id) const noexcept;
};

#ifdef UNIT_TEST
struct MakespanStub {
    virtual float operator()(State state) noexcept = 0;
    virtual ~MakespanStub() noexcept = default;
};
#endif // UNIT_TEST

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

        @param info   An instance of ScheduleInfo
        @param random An instance of RandomUtils
    */
    Mutator(const ScheduleInfo& info, RandomUtils& random);

    /**
        Mutates the scheduling configuration in-place.

        @param state Scheduling configuration
    */
    void operator()(MutState state) const noexcept;

private:
    gsl::not_null<const ScheduleInfo*> info;
    gsl::not_null<RandomUtils*> random;

    /**
        Checks if the task can be swapped with it's predecessor.

        The function checks if a predecessor is a child of a given
        task. Tasks without direct relations to each other can be
        swapped without causing scheduling conflicts.

        @param task         First task
        @param predecessor  Second task, predecessor

        @return True if tasks can be swapped
    */
    [[nodiscard]] bool
    is_swappable(int16 task, int16 predecessor) const noexcept;

    /**
        Attempts to swap two random adjacent tasks within the
        schedule.
    */
    void try_swap(MutState state) const noexcept;

    /**
        Assigns a new agent to a random task.
    */
    void update_agent(MutState state) const noexcept;
};

#ifdef UNIT_TEST
struct MutatorStub {
    virtual void operator()(MutState state) const noexcept = 0;
    virtual ~MutatorStub() noexcept = default;
};
#endif // UNIT_TEST
} // namespace angonoka::stun_dag
