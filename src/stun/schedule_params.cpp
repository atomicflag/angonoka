#include "schedule_params.h"
#include "../configuration.h"
#include <boost/container/flat_set.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

namespace {
using namespace angonoka;

using stun::ScheduleParams;
using AgentPerformance = decltype(ScheduleParams::agent_performance);
using TaskDuration = decltype(ScheduleParams::task_duration);
using AvailableAgents = decltype(ScheduleParams::available_agents);
using Dependencies = decltype(ScheduleParams::dependencies);

using ranges::to;
using ranges::actions::insert;
using ranges::views::enumerate;
using ranges::views::transform;

/**
    Construct the packed agent performance map from an array of
    Agents.

    @param agents Array of Agents

    @return Agent performance map
*/
AgentPerformance agent_performance(const Agents& agents)
{
    Expects(!agents.empty());

    return agents
        | transform([](auto&& a) { return a.performance.average(); })
        | to<AgentPerformance>();
}

/**
    Construct the packed task duration map from an array of tasks.

    Task durations are normalized to bring the ideal makespan closer
    to 1.

    @param tasks        Array of Tasks
    @param agent_count  Total number of agents

    @return Task duration map
*/
TaskDuration task_duration(const Tasks& tasks, int agent_count)
{
    Expects(!tasks.empty());
    Expects(agent_count > 0);

    TaskDuration durations;
    durations.reserve(tasks.size());
    float total{0.F};
    for (auto&& t : tasks) {
        durations.emplace_back(t.duration.average().count());
        total += durations.back();
    }
    durations.shrink_to_fit();
    const auto average_duration
        = total / static_cast<float>(agent_count);
    for (auto& d : durations) d /= average_duration;

    Ensures(durations.size() == tasks.size());

    return durations;
}

/**
    Construct the packed available agents map from Configuration.

    @param config An instance of Configuration

    @return Available agents map
*/
AvailableAgents available_agents(const Configuration& config)
{
    using stun::int16;

    Expects(!config.tasks.empty());
    Expects(!config.agents.empty());

    std::vector<int16> data;
    std::vector<int16> sizes;

    for (auto&& task : config.tasks) {
        int16 agent_count{0};
        for (auto&& [agent_index, agent] : enumerate(config.agents)) {
            if (!can_work_on(agent, task)) continue;
            ++agent_count;
            data.emplace_back(agent_index);
        }
        sizes.emplace_back(agent_count);
    }
    data.shrink_to_fit();

    Ensures(sizes.size() == config.tasks.size());

    return {std::move(data), sizes};
}

/**
    Construct the packed dependency map from Tasks.

    @param tasks Array of Tasks

    @return Dependency map.
*/
Dependencies dependencies(const Tasks& tasks)
{
    using stun::int16;

    Expects(!tasks.empty());

    std::vector<int16> data;
    std::vector<int16> sizes;

    for (auto&& task : tasks) {
        insert(data, data.end(), task.dependencies);
        sizes.emplace_back(task.dependencies.size());
    }
    data.shrink_to_fit();

    Ensures(sizes.size() == tasks.size());

    return {std::move(data), sizes};
}
} // namespace

namespace angonoka::stun {
using boost::container::flat_set;

Vector2D::Vector2D(
    std::vector<int16>&& data,
    std::vector<span<int16>>&& spans) noexcept
    : data{std::move(data)}
    , spans{std::move(spans)}
{
}

void Vector2D::clear() noexcept
{
    data.clear();
    spans.clear();
}

Vector2D::Vector2D() noexcept = default;
Vector2D::Vector2D(const Vector2D& other)
{
    if (other.empty()) return;
    data = other.data;
    spans = other.spans;
    if (data.empty()) return;
    const auto* const front_ptr = other.data.data();
    for (auto& s : spans) {
        if (s.empty()) continue;
        const auto* const p = s.data();
        const auto d = std::distance(front_ptr, p);
        s = {std::next(data.data(), d), s.size()};
    }

    Ensures(other.size() == size());
}

Vector2D::Vector2D(
    std::vector<int16>&& data,
    span<const int16> sizes) noexcept
    : data{std::move(data)}
{
    if (sizes.empty()) return;
    auto* head = this->data.data();
    for (auto&& size : sizes) {
        spans.emplace_back(
            std::exchange(head, std::next(head, size)),
            size);
    }

    Ensures(std::ssize(spans) == sizes.size());
}

[[nodiscard]] std::size_t Vector2D::size() const noexcept
{
    return spans.size();
}

[[nodiscard]] bool Vector2D::empty() const noexcept
{
    return spans.empty();
}

Vector2D& Vector2D::operator=(const Vector2D& other)
{
    *this = Vector2D{other};
    return *this;
}

Vector2D::Vector2D(Vector2D&& other) noexcept = default;
Vector2D& Vector2D::operator=(Vector2D&& other) noexcept
{
    if (&other == this) return *this;
    data = std::move(other.data);
    spans = std::move(other.spans);
    return *this;
}
Vector2D::~Vector2D() noexcept = default;

/**
    Walks the dependency tree recursively.

    @param state        Partially formed schedule
    @param tasks        Set of unexplored tasks
    @param task_index   Index of the current task
    @param params       An instance of ScheduleParams
*/
void push_task(
    std::vector<StateItem>& state,
    flat_set<int16>& tasks,
    int16 task_index,
    const ScheduleParams& params)
{
    Expects(!tasks.empty());
    Expects(
        tasks.size() + state.size() == params.task_duration.size());

    if (!tasks.contains(task_index)) return;
    const auto idx = static_cast<std::size_t>(task_index);
    for (auto&& dep_index : params.dependencies[idx])
        push_task(state, tasks, dep_index, params);
    state.emplace_back(StateItem{
        .task_id = task_index,
        .agent_id = params.available_agents[idx][0]});
    tasks.erase(task_index);

    Ensures(
        tasks.size() + state.size() == params.task_duration.size());
}

std::vector<StateItem> initial_state(const ScheduleParams& params)
{
    using ranges::front;
    using ranges::views::iota;

    Expects(!params.task_duration.empty());

    std::vector<StateItem> state;
    state.reserve(params.task_duration.size());
    auto tasks = iota(0L, std::ssize(params.task_duration))
        | ranges::to<flat_set<int16>>();
    while (!tasks.empty())
        push_task(state, tasks, front(tasks), params);

    Ensures(state.size() == params.task_duration.size());

    return state;
}

ScheduleParams to_schedule_params(const Configuration& config)
{
    Expects(!config.agents.empty());
    Expects(!config.tasks.empty());

    return {
        .agent_performance{agent_performance(config.agents)},
        .task_duration{task_duration(
            config.tasks,
            static_cast<int>(config.agents.size()))},
        .available_agents{available_agents(config)},
        .dependencies{dependencies(config.tasks)}};
}
} // namespace angonoka::stun
