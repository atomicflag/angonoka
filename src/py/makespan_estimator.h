#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>

namespace angonoka::stun {
using ranges::span;

class TaskDurations;

class MakespanEstimator {
public:
    MakespanEstimator(
        gsl::index agent_count,
        gsl::not_null<const TaskDurations*> task_durations) noexcept;

    float operator()(span<const int16> state) noexcept;

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    std::unique_ptr<float[]> makespan_buffer_data;
    span<float> makespan_buffer;
    gsl::not_null<const TaskDurations*> task_durations;
};
} // namespace angonoka::stun
