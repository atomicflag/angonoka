#include "random_utils.h"

namespace angonoka::stun {
void RandomUtils::get_neighbor(viewi v) noexcept;

float RandomUtils::get_uniform() noexcept { return r(g); }

gsl::index RandomUtils::random_index(gsl::index max) noexcept
{
    return static_cast<gsl::index>(r(g) * max);
}

std::int_fast16_t
RandomUtils::pick_random(ranges::span<std::int_fast16_t> rng) noexcept
{
    return rng[random_index(rng.size())];
}

void RandomUtils::get_neighbor(viewi v) noexcept
{
    const auto task_idx = random_index(v.size());
    v[task_idx] = pick_random(data.task_agents[task_idx]);
}

TaskAgents::TaskAgents(const std::vector<veci>& data)
    : int_data{std::make_unique<Int[]>(total_size(data))}
    , spans{std::make_unique<viewi[]>(data.size())}
    , task_agents{spans.get(), static_cast<long>(data.size())}
{
    Int* int_data_ptr = int_data.get();
    viewi* spans_ptr = spans.get();
    for (auto&& v : data) {
        *spans_ptr++ = {int_data_ptr, static_cast<long>(v.size())};
        int_data_ptr = ranges::copy(v, int_data_ptr).out;
    }
}

decltype(auto) TaskAgents::operator[](gsl::index i) const noexcept
{
    return task_agents[i];
}

TaskDurations::TaskDurations(
    const vecf& task_durations,
    const vecf& agent_performances)
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
    gsl::not_null<TaskDurations*> task_durations) noexcept
    : makespan_buffer_data(std::make_unique<float[]>(agent_count))
    , makespan_buffer(makespan_buffer_data.get(), agent_count)
    , task_durations{task_durations}
{
}

float MakespanEstimator::operator()(viewi state) noexcept
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
        const auto diff = average_stun - 0.03f;
        const auto t
            = 1.f - static_cast<float>(iteration) / max_iterations;
        value *= 1.f + diff * beta_scale * t * t;
        stun_count = 0u;
    }
}

float BetaDriver::beta() const noexcept { return value; }

float BetaDriver::last_average_stun() const noexcept
{
    return last_average;
}
} // namespace angonoka::stun
