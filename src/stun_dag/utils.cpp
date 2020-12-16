#include "utils.h"
#include "random_utils.h"
#include "schedule_info.h"
#include <range/v3/algorithm/binary_search.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/transform.hpp>

namespace angonoka::stun_dag {
Makespan::Makespan(
    gsl::not_null<const ScheduleInfo*> info)
    : info{std::move(info)}
    , sum_buffer(
info->task_duration.size()+info->agent_performance.size())
    , task_done{sum_buffer.data(), static_cast<int>(info->task_duration.size())}
    , work_done{task_done.end(), static_cast<int>(info->agent_performance.size())}
{
    Expects(!info->agent_performance.empty());
    Expects(!info->task_duration.empty());
    Ensures(!sum_buffer.empty());
    Ensures(
        task_done.size()
        == static_cast<int>(info->task_duration.size()));
    Ensures(
        work_done.size()
        == static_cast<int>(info->agent_performance.size()));
}

Makespan::Makespan(const Makespan& other)
    : Makespan{other.info}
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
Makespan& Makespan::operator=(Makespan&& other) noexcept = default;

Makespan::~Makespan() noexcept = default;

float Makespan::operator()(State state) noexcept
{
    Expects(!state.empty());
    Expects(state.size() == task_done.size());

    ranges::fill(sum_buffer, 0.F);
    for (auto [task_id, agent_id] : state) {
        const auto done = std::max(
                              dependencies_done(task_id),
                              work_done[agent_id])
            + task_duration(task_id, agent_id);
        work_done[agent_id] = task_done[task_id] = done;
    }
    return ranges::max(work_done);
}

[[nodiscard]] float
Makespan::dependencies_done(int16 task_id) const noexcept
{
    Expects(task_id >= 0);

    using ranges::views::transform;
    const auto deps
        = info->dependencies[static_cast<gsl::index>(task_id)];
    if (deps.empty()) return 0.F;
    return ranges::max(deps | transform([&](const auto& dep_id) {
                           return task_done[dep_id];
                       }));
}

[[nodiscard]] float
Makespan::task_duration(int16 task_id, int16 agent_id) const noexcept
{
    Expects(task_id >= 0);
    Expects(agent_id >= 0);
    return info->task_duration[static_cast<gsl::index>(task_id)]
        / info->agent_performance[static_cast<gsl::index>(agent_id)];
}

namespace {
    /**
        Function object for mutating the scheduling configuration.

        @var info   An instance of ScheduleInfo
        @var random An instance of RandomUtils
        @var state  Scheduling configuration
    */
    struct Mutator {
        gsl::not_null<const ScheduleInfo*> info;
        gsl::not_null<RandomUtils*> random;
        MutState state;

        /**
            Checks if the task can be swapped with it's predecessor.

            The function checks if a predecessor is a child of a given
            task. Tasks without direct relations to each other can be
            swapped without causing scheduling conflicts.

            @param task         First task
            @param predecessor  Second task, predecessor

            @return True if tasks can be swapped
        */
        [[nodiscard]] bool
        is_swappable(int16 task, int16 predecessor) const noexcept
        {
            Expects(task >= 0);
            Expects(
                static_cast<gsl::index>(task)
                < info->dependencies.size());
            Expects(predecessor >= 0);
            Expects(
                static_cast<gsl::index>(predecessor)
                < info->dependencies.size());
            Expects(task != predecessor);
            return !ranges::binary_search(
                info->dependencies[static_cast<gsl::index>(task)],
                predecessor);
        }

        /**
            Attempts to swap two random adjacent tasks within the
            schedule.
        */
        void try_swap() noexcept
        {
            Expects(state.size() >= 2);
            const auto swap_index
                = 1 + random->uniform_int(state.size() - 2);
            auto& task_a = state[swap_index].task_id;
            auto& task_b = state[swap_index - 1].task_id;
            if (!is_swappable(task_a, task_b)) return;
            std::swap(task_a, task_b);
        }

        /**
            Assigns a new agent to a random task.
        */
        void update_agent() noexcept
        {
            Expects(!state.empty());
            Expects(
                static_cast<gsl::index>(state.size())
                == info->available_agents.size());
            const auto task_index
                = random->uniform_int(state.size() - 1);
            const auto task_id
                = static_cast<gsl::index>(state[task_index].task_id);
            const auto new_agent_id = random->uniform_int(
                info->available_agents[task_id].size() - 1);
            state[task_index].agent_id = new_agent_id;
        }

        /**
            Mutates the scheduling configuration in-place.
        */
        void operator()() noexcept
        {
            Expects(!state.empty());
            const auto action = random->uniform_01();
            constexpr auto both_threshold = .6F;
            constexpr auto swap_threshold = .3F;
            if (action >= both_threshold) {
                try_swap();
                update_agent();
            } else if (action >= swap_threshold) {
                try_swap();
            } else {
                update_agent();
            }
        }
    };
} // namespace

void mutate(
    MutState state,
    const ScheduleInfo& info,
    RandomUtils& random) noexcept
{
    Expects(!state.empty());
    return Mutator{
        .info = &info,
        .random = &random,
        .state = state}();
}
} // namespace angonoka::stun_dag
