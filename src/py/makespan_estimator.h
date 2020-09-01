#pragma once

namespace angonoka::stun {
class MakespanEstimator {
public:
    MakespanEstimator(
        gsl::index agent_count,
        gsl::not_null<TaskDurations*> task_durations) noexcept;

    float operator()(viewi state) noexcept;

private:
    std::unique_ptr<float[]> makespan_buffer_data;
    ranges::span<float> makespan_buffer;
    gsl::not_null<TaskDurations*> task_durations;
};
} // namespace angonoka::stun
