#include "simulation.h"

namespace angonoka::detail {
// TODO: doc, test, expects
struct Simulation::Impl {
    // TODO: doc, test, expects
    static void assign_buffers(Simulation& self)
    {
        using ranges::span;
        const auto& agents = self.config->agents;
        const auto& tasks = self.config->tasks;

        Expects(!agents.empty());
        Expects(!tasks.empty());

        self.buffer.resize(agents.size() * 2 + tasks.size() * 2);
        const auto agent_count = std::ssize(agents);
        const auto task_count = std::ssize(tasks);
        auto* head = self.buffer.data();
        const auto next_span = [&](auto count) {
            return span{
                std::exchange(head, std::next(head, count)),
                count};
        };
        self.agent_performance = next_span(agent_count);
        self.task_duration = next_span(task_count);
        self.agent_work_end = next_span(agent_count);
        self.task_done = next_span(task_count);

        Ensures(
            self.buffer.size()
            == agents.size() * 2 + tasks.size() * 2);
        Ensures(self.agent_performance.data() == self.buffer.data());
        Ensures(self.agent_performance.size() == std::ssize(agents));
        Ensures(self.task_duration.size() == std::ssize(tasks));
        Ensures(self.agent_work_end.size() == std::ssize(agents));
        Ensures(self.task_done.size() == std::ssize(tasks));
    }

    // TODO: doc, test, expects
    static void random_agent_performances(Simulation& self)
    {
        using index = decltype(self.agent_performance)::index_type;
        for (index i = 0; auto&& agent : self.config->agents) {
            float perf = self.random->normal(
                agent.performance.min,
                agent.performance.max);
            while (perf <= 0.F) {
                perf = self.random->normal(
                    agent.performance.min,
                    agent.performance.max);
            }
            self.agent_performance[i] = perf;
            ++i;
        }
    }

    // TODO: doc, test, expects
    static void random_task_durations(Simulation& self)
    {
        using index = decltype(self.task_duration)::index_type;
        for (index i = 0; auto&& task : self.config->tasks) {
            const auto min
                = static_cast<float>(task.duration.min.count());
            const auto max
                = static_cast<float>(task.duration.max.count());
            float duration = self.random->normal(min, max);
            while (duration <= 0.F) {
                duration = self.random->normal(min, max);
            }
            self.task_duration[i] = duration;
            ++i;
        }
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
    Ensures(agent_performance.data() == buffer.data());
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
    Ensures(agent_performance.data() == buffer.data());
    Ensures(agent_performance.size() == std::ssize(config->agents));
    Ensures(task_duration.size() == std::ssize(config->tasks));
    Ensures(agent_work_end.size() == std::ssize(config->agents));
    Ensures(task_done.size() == std::ssize(config->tasks));
}

[[nodiscard]] std::chrono::seconds Simulation::operator()() noexcept
{
    Impl::random_agent_performances(*this);
    Impl::random_task_durations(*this);
    return {};
}
} // namespace angonoka::detail
