#pragma once

#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka ::stun {
    using ranges::span;
enum class AgentIndex : gsl::index {};
enum class TaskIndex : gsl::index {};

class TaskDurations {
public:
    TaskDurations() = default;
    TaskDurations(
        range<float> task_durations,
        range<float> agent_performances);

    float get(AgentIndex agent, TaskIndex task) const noexcept;

private:
    std::unique_ptr<float[]> float_data;
    gsl::index agent_count;

    gsl::index
    build_index(AgentIndex agent, TaskIndex task) const noexcept;
};

} // namespace angonoka::stun
