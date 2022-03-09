#include "simulation.h"
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/transform.hpp>

namespace angonoka::detail {
using angonoka::stun::int16;
using index_type = ranges::span<float>::index_type;

/**
    Implementation details.
*/
struct Simulation::Impl {
    /**
        Reset the buffer and assign views.

        Resizes the buffer to the new size and
        reassignes the spans to subsections of the
        new buffer.

        TODO: test
    */
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

    /**
        Picks random agent performances.

        Agent performances follow the gaussian distribution.

        TODO: test
    */
    static void random_agent_performances(Simulation& self)
    {
        Expects(!self.config->agents.empty());
        Expects(
            std::ssize(self.config->agents)
            == self.agent_performance.size());

        for (index_type i = 0; auto&& agent : self.config->agents) {
            const auto min = agent.performance.min;
            const auto max = agent.performance.max;
            if (min == max) {
                self.agent_performance[i] = min;
                ++i;
                continue;
            }
            float perf = self.random->normal(min, max);
            while (perf <= 0.F) perf = self.random->normal(min, max);
            self.agent_performance[i] = perf;
            ++i;
        }
    }

    /**
        Picks random task durations.

        Task durations follow the gaussian distribution.

        TODO: test
    */
    static void random_task_durations(Simulation& self)
    {
        Expects(!self.config->tasks.empty());
        Expects(
            std::ssize(self.config->tasks)
            == self.task_duration.size());

        for (index_type i = 0; auto&& task : self.config->tasks) {
            const auto min
                = static_cast<float>(task.duration.min.count());
            const auto max
                = static_cast<float>(task.duration.max.count());
            if (min == max) {
                self.task_duration[i] = min;
                ++i;
                continue;
            }
            float duration = self.random->normal(min, max);
            while (duration <= 0.F)
                duration = self.random->normal(min, max);
            self.task_duration[i] = duration;
            ++i;
        }
    }

    /**
        How long it will take for a given agent to complete a given
        task.

        Factors in agent's performace.

        TODO: test

        @param task_id Task's index
        @param agent_id Agent's index

        @return Time in seconds
    */
    [[nodiscard]] static float task_duration(
        const Simulation& self,
        int16 task_id,
        int16 agent_id)
    {
        Expects(agent_id >= 0);
        Expects(agent_id < std::ssize(self.config->agents));
        Expects(task_id >= 0);
        Expects(task_id < std::ssize(self.config->tasks));

        const auto performance = self.agent_performance[agent_id];
        const auto task_duration = self.task_duration[task_id];

        Expects(performance > 0.F);
        Expects(task_duration > 0.F);

        return static_cast<float>(task_duration) / performance;
    }

    /**
        The time when the last dependency of a given task will be
        completed.

        TODO: test

        @param task_id Task's index

        @return Time in seconds
    */
    [[nodiscard]] static float
    dependencies_done(const Simulation& self, const TaskIndices& deps)
    {
        Expects(!self.task_done.empty());

        using ranges::max;
        using ranges::views::transform;
        if (deps.empty()) return 0.F;
        return max(deps | transform([&](auto dep_id) {
                       return self.task_done[dep_id];
                   }));
    }
};

Simulation::Simulation(const Params& params)
    : config{params.config}
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
    return {.config{config}, .random{random}};
}

void Simulation::params(const Params& params)
{
    Expects(!config->agents.empty());
    Expects(!config->tasks.empty());

    config = params.config;
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

[[nodiscard]] std::chrono::seconds Simulation::operator()(
    ranges::span<const stun::ScheduleItem> schedule) noexcept
{
    Expects(!buffer.empty());
    Expects(!agent_performance.empty());
    Expects(!task_duration.empty());
    Expects(!agent_work_end.empty());
    Expects(!task_done.empty());
    Expects(schedule.size() == task_done.size());
    Expects(agent_performance.data() == buffer.data());

    ranges::fill(buffer, 0.F);
    Impl::random_agent_performances(*this);
    Impl::random_task_durations(*this);
    for (auto [task_id, agent_id] : schedule) {
        const auto& task
            = config->tasks[static_cast<gsl::index>(task_id)];
        const auto deps_done
            = Impl::dependencies_done(*this, task.dependencies);
        const auto done
            = std::max(deps_done, agent_work_end[agent_id])
            + Impl::task_duration(*this, task_id, agent_id);
        agent_work_end[agent_id] = task_done[task_id] = done;
    }
    using rep = std::chrono::seconds::rep;
    return std::chrono::seconds{
        static_cast<rep>(ranges::max(agent_work_end))};
}
} // namespace angonoka::detail
