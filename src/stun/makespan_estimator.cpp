#include "makespan_estimator.h"

namespace angonoka::stun {
MakespanEstimator::MakespanEstimator(
    gsl::index agent_count,
    gsl::not_null<const TaskDurations*> task_durations) noexcept
    : makespan_buffer_data(std::make_unique<float[]>(agent_count))
    , makespan_buffer(makespan_buffer_data.get(), agent_count)
    , task_durations{std::move(task_durations)}
{
}

float MakespanEstimator::operator()(span<const int16> state) noexcept
{
    ranges::fill(makespan_buffer, 0.f);
    const auto state_size = state.size();
    for (gsl::index i{0}; i < state_size; ++i) {
        const gsl::index a = state[i];
        makespan_buffer[a]
            += task_durations->get(AgentIndex{a}, TaskIndex{i});
    }
    return ranges::max(makespan_buffer);
}
} // namespace angonoka::stun
