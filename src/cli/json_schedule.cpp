#include "json_schedule.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/transform.hpp>
#include <vector>

namespace {
using namespace angonoka;

/**
    How long it will take for a given agent to complete a given
    task.

    Factors in agent's performace.

    TODO: test

    @param task     Task
    @param agent    Agent who performs the task

    @return Time in seconds
*/
[[nodiscard]] float
task_duration(const Task& task, const Agent& agent)
{
    Expects(task.duration.average().count() > 0);
    Expects(agent.performance.average() > 0.F);

    return static_cast<float>(task.duration.average().count())
        * agent.performance.average();
}

/**
    The time when the last dependency will be completed.

    TODO: test, expects

    @param task_done    Array of task completion times
    @param deps         Dependency ids

    @return Time in seconds
*/
[[nodiscard]] float dependencies_done(
    const std::vector<float>& task_done,
    const TaskIndices& deps)
{
    using ranges::max;
    using ranges::views::transform;
    if (deps.empty()) return 0.F;
    return max(deps | transform([&](auto dep_id) {
                   return task_done[dep_id];
               }));
}
} // namespace

namespace angonoka::cli {
namespace detail {
    nlohmann::json to_json(
        const Configuration& config,
        const OptimizedSchedule& schedule)
    {
        using nlohmann::json;
        json tasks;
        std::vector<int> agent_priority(config.agents.size());
        std::vector<float> agent_work_end(config.agents.size());
        std::vector<float> task_done(config.tasks.size());

        for (const auto& t : schedule.schedule) {
            const auto t_id = static_cast<gsl::index>(t.task_id);
            const auto a_id = static_cast<gsl::index>(t.agent_id);
            const auto duration = task_duration(
                config.tasks[t_id],
                config.agents[a_id]);
            const auto expected_start = std::max(
                agent_work_end[a_id],
                dependencies_done(
                    task_done,
                    config.tasks[t_id].dependencies));
            agent_work_end[a_id] = task_done[t_id]
                = expected_start + duration;

            // TODO: Refactor
            tasks.emplace_back(json{
                {"task", config.tasks[t_id].name},
                {"agent", config.agents[a_id].name},
                {"priority", agent_priority[a_id]++},
                {"expected_duration", static_cast<int>(duration)},
                {"expected_start", expected_start}});
        }

        return {
            {"makespan", schedule.makespan.count()},
            {"tasks", std::move(tasks)}};
    }
} // namespace detail

nlohmann::json json_schedule(
    const Configuration& config,
    const Options& /* options */)
{
    Expects(!config.tasks.empty());
    Expects(!config.agents.empty());

    // TODO: WIP: Implement
    return {};
}
} // namespace angonoka::cli
