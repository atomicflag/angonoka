#include "simulation.h"

namespace angonoka::detail {
// TODO: doc, test, expects
struct Simulation::Impl {
    // TODO: doc, test, expects
    static void assign_buffers(Simulation& self)
    {
        Expects(!self.config->agents.empty());
        Expects(!self.config->tasks.empty());

        self.buffer.resize(
            self.config->agents.size() * 2
            + self.config->tasks.size() * 2);
        const auto agent_count = std::ssize(self.config->agents);
        const auto task_count = std::ssize(self.config->tasks);
        auto* head = self.buffer.data();
        auto assign_span = [&](auto& span, auto count) {
            span
                = {std::exchange(head, std::next(head, count)),
                   count};
        };
        assign_span(self.agent_performance, agent_count);
        assign_span(self.task_duration, task_count);
        assign_span(self.agent_work_end, agent_count);
        assign_span(self.task_done, task_count);

        Ensures(
            self.buffer.size()
            == self.config->agents.size() * 2
                + self.config->tasks.size() * 2);
        Ensures(
            self.agent_performance.size()
            == std::ssize(self.config->agents));
        Ensures(
            self.task_duration.size()
            == std::ssize(self.config->tasks));
        Ensures(
            self.agent_work_end.size()
            == std::ssize(self.config->agents));
        Ensures(
            self.task_done.size() == std::ssize(self.config->tasks));
    }
};

Simulation::Simulation(const Params& params)
    : config{params.config}
    , schedule{params.schedule}
    , random{params.random}
{
    Expects(!config->agents.empty());
    Expects(!config->tasks.empty());

    Impl::assign_buffers(*this);

    Ensures(
        buffer.size()
        == config->agents.size() * 2 + config->tasks.size() * 2);
    Ensures(agent_performance.size() == std::ssize(config->agents));
    Ensures(task_duration.size() == std::ssize(config->tasks));
    Ensures(agent_work_end.size() == std::ssize(config->agents));
    Ensures(task_done.size() == std::ssize(config->tasks));
}

[[nodiscard]] auto Simulation::params() const -> Params
{
    return {.config{config}, .schedule{schedule}, .random{random}};
}

void Simulation::params(const Params& params)
{
    Expects(!config->agents.empty());
    Expects(!config->tasks.empty());

    config = params.config;
    schedule = params.schedule;
    random = params.random;
    Impl::assign_buffers(*this);

    Ensures(
        buffer.size()
        == config->agents.size() * 2 + config->tasks.size() * 2);
    Ensures(agent_performance.size() == std::ssize(config->agents));
    Ensures(task_duration.size() == std::ssize(config->tasks));
    Ensures(agent_work_end.size() == std::ssize(config->agents));
    Ensures(task_done.size() == std::ssize(config->tasks));
}

[[nodiscard]] std::chrono::seconds Simulation::operator()() noexcept
{
    return {};
}
} // namespace angonoka::detail
