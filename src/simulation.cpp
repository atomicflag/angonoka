#include "simulation.h"

namespace angonoka::detail {
Simulation::Simulation(
    const Configuration& config,
    const OptimizedSchedule& schedule,
    stun::RandomUtils& random)
    : config{&config}
    , schedule{&schedule}
    , random{&random}
    , buffer(config.agents.size() * 2 + config.tasks.size() * 2)
{
    Expects(
        buffer.size()
        == config.agents.size() * 2 + config.tasks.size() * 2);

    const auto agent_count = std::ssize(config.agents);
    const auto task_count = std::ssize(config.tasks);
    auto* head = buffer.data();
    auto assign_span = [&](auto& span, auto count) {
        span = {std::exchange(head, std::next(head, count)), count};
    };
    assign_span(agent_performance, agent_count);
    assign_span(task_duration, task_count);
    assign_span(agent_work_end, agent_count);
    assign_span(task_done, task_count);

    Ensures(agent_performance.size() == std::ssize(config.agents));
    Ensures(task_duration.size() == std::ssize(config.tasks));
    Ensures(agent_work_end.size() == std::ssize(config.agents));
    Ensures(task_done.size() == std::ssize(config.tasks));
}

[[nodiscard]] std::chrono::seconds Simulation::operator()() noexcept
{
    return {};
}
} // namespace angonoka::detail
