#pragma once

#include "common.h"
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun_dag {
using ranges::span;

/**
    General, read-only information about the schedule.

    @var agent_performance      Agent's speed multipliers
    @var task_duration          Task durations in seconds
    @var available_agents_data  Cache-friendly buffer
    @var available_agents       Which agents can perform each task
    @var dependencies_data      Cache-friendly buffer
    @var dependencies           Task's dependent sub-tasks
*/
struct ScheduleInfo {
    std::vector<float> agent_performance;
    std::vector<float> task_duration;

    std::vector<int16> available_agents_data;
    std::vector<span<int16>> available_agents;

    std::vector<int16> dependencies_data;
    std::vector<span<int16>> dependencies;
};

} // namespace angonoka::stun_dag
