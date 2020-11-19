#include "utils.h"
#include "random_utils.h"
#include "schedule_info.h"
#include <range/v3/algorithm/binary_search.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/transform.hpp>

namespace angonoka::stun_dag {

Makespan::Makespan(
    gsl::not_null<const ScheduleInfo*> info,
    TasksCount tasks_count,
    AgentsCount agents_count)
    : info{std::move(info)}
    , sum_buffer(
          static_cast<gsl::index>(tasks_count)
          + static_cast<gsl::index>(agents_count))
    , task_done{sum_buffer.data(), static_cast<int>(tasks_count)}
    , work_done{task_done.end(), static_cast<int>(agents_count)}
{
    Expects(static_cast<int>(tasks_count) > 0);
    Expects(static_cast<int>(agents_count) > 0);
    Ensures(!sum_buffer.empty());
}

Makespan::Makespan(const Makespan& other)
    : Makespan{
        other.info,
        static_cast<TasksCount>(other.task_done.size()),
        static_cast<AgentsCount>(other.work_done.size())}
{
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
        const auto done
            = std::max(dependency_done(task_id), work_done[agent_id])
            + task_duration(task_id, agent_id);
        work_done[agent_id] = task_done[task_id] = done;
    }
    return ranges::max(work_done);
}

[[nodiscard]] float
Makespan::dependency_done(int16 task_id) const noexcept
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
        TODO: doc

        @var info
        @var random
    */
    struct Mutator {
        gsl::not_null<const ScheduleInfo*> info;
        gsl::not_null<RandomUtils*> random;

        /**
            TODO: doc

            @param index

            @return
        */
        [[nodiscard]] bool is_swappable(int16 index) const noexcept
        {
            Expects(index >= 1);
            return !ranges::binary_search(
                info->dependencies[static_cast<gsl::index>(index)],
                index - 1);
        }

        /**
            TODO: doc

            @param state
        */
        void try_swap(MutState state) const noexcept
        {
            Expects(!state.empty());
            const auto swap_index
                = 1 + random->uniform_int(state.size() - 2);
            if (!is_swappable(swap_index)) return;
            using std::swap;
            swap(state[swap_index], state[swap_index - 1]);
        }

        /**
            TODO: doc

            @param state
        */
        void update_agent(MutState state) const noexcept
        {
            Expects(!state.empty());
            const auto task_index
                = random->uniform_int(state.size() - 1);
            const auto task_id
                = static_cast<gsl::index>(state[task_index].task_id);
            const auto new_agent_id = random->uniform_int(
                info->available_agents[task_id].size() - 1);
            state[task_index].agent_id = new_agent_id;
        }

        /**
            TODO: doc

            @param state
        */
        void operator()(MutState state) const noexcept
        {
            Expects(!state.empty());
            try_swap(state);
            update_agent(state);
        }
    };
} // namespace

void mutate(
    const ScheduleInfo& info,
    RandomUtils& random,
    MutState state) noexcept
{
    return Mutator{.info = &info, .random = &random}(state);
}
} // namespace angonoka::stun_dag
