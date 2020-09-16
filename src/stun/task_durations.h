#pragma once

#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>

namespace angonoka::stun {
using ranges::span;

// TODO: Add docstrings

enum class AgentIndex : gsl::index {};
enum class TaskIndex : gsl::index {};

class TaskDurations {
public:
    TaskDurations() = default;
    TaskDurations(
        span<const float> task_durations,
        span<const float> agent_performances);

    [[nodiscard]] float
    get(AgentIndex agent, TaskIndex task) const noexcept;

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    std::unique_ptr<float[]> float_data;
    gsl::index agent_count;

    [[nodiscard]] gsl::index
    build_index(AgentIndex agent, TaskIndex task) const noexcept;
};

} // namespace angonoka::stun
