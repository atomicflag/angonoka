#include "schedule_info.h"
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

using stun::ScheduleInfo;
using AgentPerformance = decltype(ScheduleInfo::agent_performance);
using TaskDuration = decltype(ScheduleInfo::task_duration);
using AvailableAgents = decltype(ScheduleInfo::available_agents);
using Dependencies = decltype(ScheduleInfo::dependencies);

using ranges::to;
using ranges::actions::insert;
using ranges::views::enumerate;
using ranges::views::transform;

// TODO: doc, test
AgentPerformance agent_performance(const Agents& agents)
{
    Expects(!agents.empty());

    return agents
        | transform([](auto&& a) { return a.performance.average(); })
        | to<AgentPerformance>();
}

// TODO: doc, test
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

// TODO: doc, test
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

// TODO: doc, test
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

VectorOfSpans::VectorOfSpans(
    std::vector<int16>&& data,
    std::vector<span<int16>>&& spans) noexcept
    : data{std::move(data)}
    , spans{std::move(spans)}
{
}

void VectorOfSpans::clear() noexcept
{
    data.clear();
    spans.clear();
}

VectorOfSpans::VectorOfSpans() noexcept = default;
VectorOfSpans::VectorOfSpans(const VectorOfSpans& other)
{
    if (other.empty()) return;
    data = other.data;
    spans = other.spans;
    auto* const front_ptr = other.spans.front().data();
    for (auto& s : spans) {
        if (s.empty()) continue;
        const auto d = std::distance(front_ptr, s.data());
        s = {std::next(data.data(), d), s.size()};
    }

    Ensures(other.size() == size());
}

VectorOfSpans::VectorOfSpans(
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

[[nodiscard]] std::size_t VectorOfSpans::size() const noexcept
{
    return spans.size();
}

[[nodiscard]] bool VectorOfSpans::empty() const noexcept
{
    return spans.empty();
}

VectorOfSpans& VectorOfSpans::operator=(const VectorOfSpans& other)
{
    *this = VectorOfSpans{other};
    return *this;
}

VectorOfSpans::VectorOfSpans(
    VectorOfSpans&& other) noexcept = default;
VectorOfSpans&
VectorOfSpans::operator=(VectorOfSpans&& other) noexcept
{
    if (&other == this) return *this;
    data = std::move(other.data);
    spans = std::move(other.spans);
    return *this;
}
VectorOfSpans::~VectorOfSpans() noexcept = default;

/**
    Walks the dependency tree recursively.

    @param state        Partially formed schedule
    @param tasks        Set of unexplored tasks
    @param task_index   Index of the current task
    @param info         An instance of ScheduleInfo
*/
void push_task(
    std::vector<StateItem>& state,
    flat_set<int16>& tasks,
    int16 task_index,
    const ScheduleInfo& info)
{
    Expects(!tasks.empty());
    Expects(tasks.size() + state.size() == info.task_duration.size());

    if (!tasks.contains(task_index)) return;
    const auto idx = static_cast<std::size_t>(task_index);
    for (auto&& dep_index : info.dependencies[idx])
        push_task(state, tasks, dep_index, info);
    state.emplace_back(StateItem{
        .task_id = task_index,
        .agent_id = info.available_agents[idx][0]});
    tasks.erase(task_index);

    Ensures(tasks.size() + state.size() == info.task_duration.size());
}

std::vector<StateItem> initial_state(const ScheduleInfo& info)
{
    using ranges::front;
    using ranges::views::iota;

    Expects(!info.task_duration.empty());

    std::vector<StateItem> state;
    state.reserve(info.task_duration.size());
    auto tasks = iota(0L, std::ssize(info.task_duration))
        | ranges::to<flat_set<int16>>();
    while (!tasks.empty())
        push_task(state, tasks, front(tasks), info);

    Ensures(state.size() == info.task_duration.size());

    return state;
}

// TODO: doc, test, expects
ScheduleInfo to_schedule(const Configuration& config)
{
    return {
        .agent_performance{agent_performance(config.agents)},
        .task_duration{task_duration(
            config.tasks,
            static_cast<int>(config.agents.size()))},
        .available_agents{available_agents(config)},
        .dependencies{dependencies(config.tasks)}};
}
} // namespace angonoka::stun
