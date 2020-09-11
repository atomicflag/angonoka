#include "beta_driver.h"
#include "makespan_estimator.h"
#include "random_utils.h"
#include "task_agents.h"
#include "task_durations.h"
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <utility>

namespace {
constexpr std::uint_fast32_t average_stun_window
    = angonoka::stun::max_iterations / 100;
} // namespace

namespace angonoka::stun {
float RandomUtils::get_uniform() noexcept { return r(g); }

RandomUtils::index_type RandomUtils::random_index(index_type max) noexcept
{
    return static_cast<index_type>(r(g) * static_cast<float>(max));
}

int16 RandomUtils::pick_random(span<const int16> rng) noexcept
{
    return rng[random_index(rng.size())];
}

void RandomUtils::get_neighbor(span<int16> v) noexcept
{
    const auto task_idx = random_index(v.size());
    // TODO: Fix this
    // v[task_idx] = pick_random(data.task_agents[task_idx]);
}

TaskAgents::TaskAgents(span<const int16> data)
    // TODO: We can't invoce total_size here, data isn't what we expected for some reason
    : int_data{std::make_unique<int16[]>(total_size(data))}
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    , spans{std::make_unique<span<const int16>[]>(data.size())}
    , task_agents{spans.get(), static_cast<long>(data.size())}
{
    int16* int_data_ptr = int_data.get();
    span<int16>* spans_ptr = spans.get();
    for (auto&& v : data) {
        *spans_ptr++ = {int_data_ptr, static_cast<long>(v.size())};
        int_data_ptr = ranges::copy(v, int_data_ptr).out;
    }
}

TaskDurations::TaskDurations(
    span<const float> task_durations,
    span<const float> agent_performances)
    : float_data{std::make_unique<float[]>(
        task_durations.size() * agent_performances.size())}
    , agent_count{agent_performances.size()}
{
    for (gsl::index i{0}; i < task_durations.size(); ++i) {
        for (gsl::index j{0}; j < agent_performances.size(); ++j) {
            const auto idx = build_index(AgentIndex{j}, TaskIndex{i});
            float_data[idx]
                = task_durations[i] / agent_performances[j];
        }
    }
}

float TaskDurations::get(AgentIndex agent, TaskIndex task)
    const noexcept
{
    return float_data[build_index(agent, task)];
}

gsl::index TaskDurations::build_index(
    AgentIndex agent,
    TaskIndex task) const noexcept
{
    return static_cast<gsl::index>(task) * agent_count
        + static_cast<gsl::index>(agent);
}

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

BetaDriver::BetaDriver(float beta, float beta_scale)
    : value{beta}
    , beta_scale{beta_scale}
{
}

void BetaDriver::update(
    float stun,
    std::uint_fast64_t iteration) noexcept
{
    average_stun += stun;
    if (++stun_count == average_stun_window) {
        average_stun /= stun_count;
        last_average = average_stun;
        const auto diff = average_stun - 0.03F;
        const auto t
            = 1.F - static_cast<float>(iteration) / max_iterations;
        value *= 1.F + diff * beta_scale * t * t;
        stun_count = 0U;
    }
}

[[nodiscard]] float BetaDriver::beta() const noexcept
{
    return value;
}

[[nodiscard]] float BetaDriver::last_average_stun() const noexcept
{
    return last_average;
}
} // namespace angonoka::stun
