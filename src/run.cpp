#include "run.h"
#include "config/load.h"
#include "stun/common.h"
#include "stun/random_utils.h"
#include "stun/schedule_info.h"
#include "stun/stochastic_tunneling.h"
#include "stun/temperature.h"
#include "stun/utils.h"
#include <fmt/ranges.h>
#include <gsl/gsl-lite.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/transform.hpp>
#include <utility>
#include <vector>

namespace angonoka {
using stun::ScheduleInfo;
using AgentPerformance = decltype(ScheduleInfo::agent_performance);
using TaskDuration = decltype(ScheduleInfo::task_duration);
using AvailableAgents = decltype(ScheduleInfo::available_agents);
using Dependencies = decltype(ScheduleInfo::dependencies);

using ranges::front;
using ranges::to;
using ranges::actions::insert;
using ranges::views::enumerate;
using ranges::views::iota;
using ranges::views::transform;

// TODO: doc, test, expects
AgentPerformance average(const Agents& agents)
{
    return agents
        | transform([](auto&& a) { return a.performance.average(); })
        | to<AgentPerformance>();
}

// TODO: doc, test, expects
TaskDuration average(const Tasks& tasks)
{
    return tasks | transform([](auto&& a) {
               return a.duration.average().count();
           })
        | to<TaskDuration>();
}

// TODO: doc, test, expects
AvailableAgents available_agents(const System& sys)
{
    using stun::int16;

    std::vector<int16> data;
    std::vector<int16> sizes;

    for (auto&& task : sys.tasks) {
        int16 agent_count{0};
        for (auto&& [agent_index, agent] : enumerate(sys.agents)) {
            if (!can_work_on(agent, task)) continue;
            ++agent_count;
            data.emplace_back(agent_index);
        }
        sizes.emplace_back(agent_count);
    }
    data.shrink_to_fit();
    return {std::move(data), sizes};
}

// TODO: doc, test, expects
Dependencies dependencies(const Tasks& tasks)
{
    using stun::int16;

    std::vector<int16> data;
    std::vector<int16> sizes;

    for (auto&& task : tasks) {
        insert(data, data.end(), task.dependencies);
        sizes.emplace_back(task.dependencies.size());
    }
    data.shrink_to_fit();
    return {std::move(data), sizes};
}

// TODO: doc, test, expects
stun::ScheduleInfo to_schedule(const System& sys)
{
    return {
        .agent_performance{average(sys.agents)},
        .task_duration{average(sys.tasks)},
        .available_agents{available_agents(sys)},
        .dependencies{dependencies(sys.tasks)}};
}

// TODO: doc, test, expects
void push_task(
    std::vector<stun::StateItem>& state,
    TaskIndices& tasks,
    TaskIndex task_index,
    const stun::ScheduleInfo& info)
{
    if (!tasks.contains(task_index)) return;
    const auto idx = static_cast<std::size_t>(task_index);
    for (auto&& dep_index : info.dependencies[idx])
        push_task(state, tasks, dep_index, info);
    state.emplace_back(stun::StateItem{
        .task_id = task_index,
        .agent_id = info.available_agents[idx][0]});
    tasks.erase(task_index);
}

// TODO: doc, test, expects
std::vector<stun::StateItem>
initial_state(const stun::ScheduleInfo& info)
{
    std::vector<stun::StateItem> state;
    auto tasks = iota(0L, std::ssize(info.task_duration))
        | to<TaskIndices>();
    while (!tasks.empty())
        push_task(state, tasks, front(tasks), info);

    Ensures(!state.empty());
    Ensures(state.size() == info.task_duration.size());

    return state;
}

void run(std::string_view tasks_yml)
{
    using namespace angonoka::stun;

    // TODO: Handle YAML exceptions
    const auto system = load_file(tasks_yml);
    const auto schedule = to_schedule(system);
    auto state = initial_state(schedule);
    float beta = 1.0F;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"
    constexpr auto number_of_epochs = 10;
    constexpr auto beta_scale = 1e-4F;
    constexpr auto stun_window = 10000;
    constexpr auto gamma = .5F;

    for (int i{0}; i < number_of_epochs; ++i) {
        RandomUtils random_utils;
        Mutator mutator{schedule, random_utils};
        Makespan makespan{schedule};
        Temperature temperature{
            Beta{beta},
            BetaScale{beta_scale},
            StunWindow{stun_window}};
        auto r = stochastic_tunneling(
            state,
            STUNOptions{
                .mutator{&mutator},
                .random{&random_utils},
                .makespan{&makespan},
                .temp{&temperature},
                .gamma{gamma}});
        state = std::move(r.state);
        beta = r.temperature;
    }
    fmt::print("{}\n", state);
#pragma clang diagnostic pop
}
} // namespace angonoka
