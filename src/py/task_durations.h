#pragma once

namespace angonoka ::stun {
enum class AgentIndex : gsl::index {};
enum class TaskIndex : gsl::index {};

class TaskDurations {
public:
    TaskDurations() = default;
    TaskDurations(
        const vecf& task_durations,
        const vecf& agent_performances);

    float get(AgentIndex agent, TaskIndex task) const noexcept;

private:
    std::unique_ptr<float[]> float_data;
    gsl::index agent_count;

    gsl::index
    build_index(AgentIndex agent, TaskIndex task) const noexcept;
};

} // namespace angonoka::stun
