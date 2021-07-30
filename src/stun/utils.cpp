#include "utils.h"
#include "random_utils.h"
#include "schedule_params.h"
#include <range/v3/algorithm/binary_search.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/transform.hpp>

namespace angonoka::stun {
/**
    Implementation details.
*/
struct Makespan::Impl {
    /**
        The time when the last dependency of a given task will be
        completed.

        @param task_id Task's index

        @return Time in seconds
    */
    [[nodiscard]] static float
    dependencies_done(Makespan& self, int16 task_id) noexcept
    {
        Expects(task_id >= 0);

        using ranges::views::transform;
        const auto deps
            = self.params_
                  ->dependencies[static_cast<gsl::index>(task_id)];
        if (deps.empty()) return 0.F;
        return ranges::max(deps | transform([&](const auto& dep_id) {
                               return self.task_done[dep_id];
                           }));
    }

    /**
        How long it will take for a given agent to complete a given
        task.

        Factors in agent's performace.

        @param task_id Task's index
        @param agent_id Agent's index

        @return Time in seconds
    */
    [[nodiscard]] static float task_duration(
        Makespan& self,
        int16 task_id,
        int16 agent_id) noexcept
    {
        Expects(task_id >= 0);
        Expects(agent_id >= 0);
        return self.params_
                   ->task_duration[static_cast<gsl::index>(task_id)]
            / self.params_->agent_performance[static_cast<gsl::index>(
                agent_id)];
    }
};

Makespan::Makespan(const ScheduleParams& params)
    : params_{&params}
    , sum_buffer(
          params.task_duration.size() + params.agent_performance.size())
    , task_done{sum_buffer.data(), static_cast<int>(params.task_duration.size())}
    , work_done{
          task_done.end(),
          static_cast<int>(params.agent_performance.size())}
{
    Expects(!params.agent_performance.empty());
    Expects(!params.task_duration.empty());
    Ensures(!sum_buffer.empty());
    Ensures(
        task_done.size()
        == static_cast<int>(params.task_duration.size()));
    Ensures(
        work_done.size()
        == static_cast<int>(params.agent_performance.size()));
}

void Makespan::params(const ScheduleParams& params)
{
    params_ = &params;
}

const ScheduleParams& Makespan::params() const { return *params_; }

Makespan::Makespan(const Makespan& other)
    : Makespan{*other.params_}
{
    Ensures(sum_buffer.size() == other.sum_buffer.size());
}

Makespan& Makespan::operator=(const Makespan& other) noexcept
{
    *this = Makespan{other};
    Ensures(sum_buffer.size() == other.sum_buffer.size());
    return *this;
}

Makespan::Makespan(Makespan&& other) noexcept = default;
Makespan& Makespan::operator=(Makespan&& other) noexcept
{
    if (&other == this) return *this;

    params_ = std::move(other.params_);
    sum_buffer = std::move(other.sum_buffer);
    task_done = other.task_done;
    work_done = other.work_done;

    Ensures(!sum_buffer.empty());
    return *this;
}

Makespan::~Makespan() noexcept = default;

float Makespan::operator()(Schedule schedule) noexcept
{
    Expects(!schedule.empty());
    Expects(schedule.size() == task_done.size());
    Expects(!sum_buffer.empty());
    Expects(task_done.data() == sum_buffer.data());

    ranges::fill(sum_buffer, 0.F);
    for (auto [task_id, agent_id] : schedule) {
        const auto done = std::max(
                              Impl::dependencies_done(*this, task_id),
                              work_done[agent_id])
            + Impl::task_duration(*this, task_id, agent_id);
        work_done[agent_id] = task_done[task_id] = done;
    }
    return ranges::max(work_done);
}

/**
    Implementation details.
*/
struct Mutator::Impl {
    /**
        Checks if the task can be swapped with it's predecessor.

        The function checks if a predecessor is a child of a given
        task. Tasks without direct relations to each other can be
        swapped without causing scheduling conflicts.

        @param task         First task
        @param predecessor  Second task, predecessor

        @return True if tasks can be swapped
    */
    [[nodiscard]] static bool is_swappable(
        const Mutator& self,
        int16 task,
        int16 predecessor) noexcept
    {
        Expects(task >= 0);
        Expects(
            static_cast<gsl::index>(task)
            < self.params->dependencies.size());
        Expects(predecessor >= 0);
        Expects(
            static_cast<gsl::index>(predecessor)
            < self.params->dependencies.size());
        Expects(task != predecessor);
        return !ranges::binary_search(
            self.params->dependencies[static_cast<gsl::index>(task)],
            predecessor);
    }

    /**
        Attempts to swap two random adjacent tasks within the
        schedule.
    */
    static void
    try_swap(const Mutator& self, MutSchedule schedule) noexcept
    {
        Expects(!schedule.empty());
        if (schedule.size() == 1) return;
        const auto swap_index
            = 1 + self.random->uniform_int(schedule.size() - 2);
        auto& task_a = schedule[swap_index];
        auto& task_b = schedule[swap_index - 1];
        if (!is_swappable(self, task_a.task_id, task_b.task_id))
            return;
        std::swap(task_a, task_b);
    }

    /**
        Assigns a new agent to a random task.
    */
    static void
    update_agent(const Mutator& self, MutSchedule schedule) noexcept
    {
        Expects(!schedule.empty());
        const auto& available_agents = self.params->available_agents;
        Expects(
            static_cast<gsl::index>(schedule.size())
            == available_agents.size());

        const auto task_index
            = self.random->uniform_int(schedule.size() - 1);
        const auto task_id
            = static_cast<gsl::index>(schedule[task_index].task_id);
        const auto new_agent_id = self.random->uniform_int(
            available_agents[task_id].size() - 1);
        schedule[task_index].agent_id
            = available_agents[task_id][new_agent_id];
    }
};

Mutator::Mutator(const ScheduleParams& params, RandomUtils& random)
    : params{&params}
    , random{&random}
{
    Expects(!params.dependencies.empty());
    Expects(!params.available_agents.empty());
}

Mutator::Mutator(const Options& options)
    : Mutator{*options.params, *options.random}
{
}

auto Mutator::options() const -> Options
{
    return {.params{params}, .random{random}};
}

void Mutator::options(const Options& options)
{
    params = options.params;
    random = options.random;
}

void Mutator::operator()(MutSchedule schedule) const noexcept
{
    Expects(!schedule.empty());
    Impl::try_swap(*this, schedule);
    Impl::update_agent(*this, schedule);
}
} // namespace angonoka::stun
