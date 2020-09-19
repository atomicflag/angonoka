#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>

// TODO: Docs and tests

namespace angonoka::stun {
using ranges::span;

class TaskDurationCache;

class MakespanEstimator {
public:
    MakespanEstimator(
        gsl::index agent_count,
        gsl::not_null<const TaskDurationCache*>
            task_duration_cache) noexcept;

    float operator()(span<const int16> state) noexcept;

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    std::unique_ptr<float[]> makespan_buffer_data;
    span<float> makespan_buffer;
    gsl::not_null<const TaskDurationCache*> task_duration_cache;
};
} // namespace angonoka::stun
