#pragma once

#include <vector>

namespace angonoka {
struct Configuration;
} // namespace angonoka

namespace angonoka::stun {
struct ScheduleParams {
    std::vector<float> agent_performance;
    std::vector<float> task_duration;
    std::vector<float> available_agents;
    std::vector<float> dependencies;
    float duration_multiplier;
};

ScheduleParams to_schedule_params(const Configuration& config);
} // namespace angonoka::stun
