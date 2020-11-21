#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun_dag {
using ranges::span;
struct ScheduleInfo;

enum class TasksCount : int {};
enum class AgentsCount : int {};

/**
    Stateful function object for calculating the makespan.

    Further reading:
    https://en.wikipedia.org/wiki/Makespan

    @var info       An instance of ScheduleInfo
    @var sum_buffer Cache-friendly buffer
    @var task_done  Mutable view of completion times for individual
                    tasks
    @var work_done  Mutable view of completion times for individual
                    agents
*/
class Makespan {
public:
    /**
        Constructor.

        @param info         An instance of ScheduleInfo
        @param tasks_count  Total number of tasks
        @param agents_count Total number of agents
    */
    Makespan(
        gsl::not_null<const ScheduleInfo*> info,
        TasksCount tasks_count,
        AgentsCount agents_count);

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

class RandomUtils;

/**
    Shuffle tasks and agents in-place.

    Randomly swaps two adjacent tasks within the schedule and
    reassigns an agent of a random task.

    @param info     An instance of ScheduleInfo
    @param random   An instance of RandomUtils
    @param state    Scheduling configuration
*/
void mutate(
    const ScheduleInfo& info,
    RandomUtils& random,
    MutState state) noexcept;
} // namespace angonoka::stun_dag
