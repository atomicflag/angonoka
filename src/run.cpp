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
#include <indicators/progress_bar.hpp>
#include <indicators/terminal_size.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/enumerate.hpp>
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

// TODO: doc, test, expects
stun::ScheduleInfo to_schedule(const Configuration& config)
{
    return {
        .agent_performance{agent_performance(config.agents)},
        .task_duration{task_duration(
            config.tasks,
            static_cast<int>(config.agents.size()))},
        .available_agents{available_agents(config)},
        .dependencies{dependencies(config.tasks)}};
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"

/**
    Find the optimal schedule.

    TODO: test, expects

    @param info Instance of ScheduleInfo

    @return Optimal schedule.
*/
std::vector<stun::StateItem> optimize(const stun::ScheduleInfo& info)
{
    Expects(!info.agent_performance.empty());
    Expects(!info.task_duration.empty());
    Expects(!info.available_agents.empty());
    Expects(
        info.available_agents.size()
        == info.agent_performance.size());

    using namespace angonoka::stun;

    auto state = initial_state(info);
    float beta = 1.0F;
    constexpr auto number_of_epochs = 10;
    constexpr auto beta_scale = 1e-4F;
    constexpr auto stun_window = 10000;
    constexpr auto gamma = .5F;

    RandomUtils random_utils;
    Mutator mutator{info, random_utils};
    Makespan makespan{info};
    Temperature temperature{
        Beta{beta},
        BetaScale{beta_scale},
        StunWindow{stun_window}};

    for (int i{0}; i < number_of_epochs; ++i) {
        auto r = stochastic_tunneling(
            state,
            STUNOptions{
                .mutator{&mutator},
                .random{&random_utils},
                .makespan{&makespan},
                .temp{&temperature},
                .gamma{gamma}});
        state = std::move(r.state);
    }

    // TODO: track progress via progress bar
    // TODO: return the Result, not just the state
    return state;
}
#pragma clang diagnostic pop

void run(std::string_view tasks_yml)
{
    using namespace angonoka::stun;

    // TODO: Handle YAML exceptions
    const auto config = load_file(tasks_yml);
    const auto schedule = to_schedule(config);
    const auto state = optimize(schedule);
    fmt::print("{}\n", state);
}
} // namespace angonoka
