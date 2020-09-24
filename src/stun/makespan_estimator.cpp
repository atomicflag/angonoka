#include "makespan_estimator.h"
#include "task_duration_cache.h"
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>

namespace angonoka::stun {
MakespanEstimator::MakespanEstimator(
    gsl::index agent_count,
    gsl::not_null<const TaskDurationCache*>
        task_duration_cache) noexcept
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    : makespan_buffer_data(std::make_unique<float[]>(agent_count))
    , makespan_buffer(
          makespan_buffer_data.get(),
          static_cast<index>(agent_count))
    , task_duration_cache{std::move(task_duration_cache)}
{
    Expects(agent_count > 0);

    Ensures(makespan_buffer_data);
    Ensures(!makespan_buffer.empty());
}

float MakespanEstimator::operator()(span<const int16> state) noexcept
{
    Expects(!state.empty());

    ranges::fill(makespan_buffer, 0.F);
    const auto state_size = state.size();
    for (index i{0}; i < state_size; ++i) {
        const index a = state[i];
        makespan_buffer[a]
            += task_duration_cache->get(AgentIndex{a}, TaskIndex{i});
    }
    return ranges::max(makespan_buffer);
}
} // namespace angonoka::stun
