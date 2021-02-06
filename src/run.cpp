#include "run.h"
#include "config/load.h"
#include "stun/common.h"
#include "stun/random_utils.h"
#include "stun/schedule_info.h"
#include "stun/stochastic_tunneling.h"
#include "stun/temperature.h"
#include "stun/utils.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/to_container.hpp>
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

using ranges::to;
using ranges::actions::insert;
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
        for (int16 agent_index{0};
             agent_index < std::ssize(sys.agents);
             ++agent_index) {
            const auto& agent
                = sys.agents[static_cast<std::size_t>(agent_index)];
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

void run(std::string_view tasks_yml)
{
    using namespace angonoka::stun;

    // TODO: Handle YAML exceptions
    const auto system = load_file(tasks_yml);
    const auto schedule = to_schedule(system);

    // TODO: Generate initial state

    /*
    float beta = 1.0F;
    for (int i{0}; i < 10; ++i) {

        RandomUtils random_utils;
        Mutator mutator{schedule, random_utils};
        Makespan makespan{schedule};
        Temperature temperature{
            Beta{beta},
            BetaScale{1e-4f},
            StunWindow{10000}};
        auto r = stochastic_tunneling(
            state,
            STUNOptions{
                .mutator{&mutator},
                .random{&random_utils},
                .makespan{&makespan},
                .temp{&temperature},
                .gamma{.5F}});
        state = std::move(r.state);
        beta = r.temperature;
    }
    fmt::print("{}\n", state);
    */
}
} // namespace angonoka
