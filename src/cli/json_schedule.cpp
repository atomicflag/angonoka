#include "json_schedule.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/transform.hpp>
#include <vector>
#ifndef NDEBUG
#include <boost/safe_numerics/safe_integer.hpp>
#include <iosfwd>
#endif // NDEBUG

namespace angonoka::cli {
namespace detail {
#ifndef NDEBUG
    namespace sn = boost::safe_numerics;
    using int16 = sn::safe<std::int_fast16_t>;
#else // NDEBUG
    using int16 = std::int_fast16_t;
#endif // NDEBUG

    // TODO: doc, test, expects
    [[nodiscard]] float
    task_duration(const Task& task, const Agent& agent)
    {
        return static_cast<float>(task.duration.average().count())
            * agent.performance.average();
    }

    // TODO: doc, test, expects
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
            const auto& task
                = config.tasks[static_cast<gsl::index>(t.task_id)];
            const auto& agent
                = config.agents[static_cast<gsl::index>(t.agent_id)];
            const auto duration = task_duration(task, agent);
            const auto priority
                = agent_priority[static_cast<gsl::index>(
                    t.agent_id)]++;
            auto& work_end
                = agent_work_end[static_cast<gsl::index>(t.agent_id)];
            const auto expected_start = std::max(
                work_end,
                dependencies_done(task_done, task.dependencies));
            work_end = task_done[static_cast<gsl::index>(t.task_id)]
                = expected_start + duration;

            // TODO: Refactor
            tasks.emplace_back(json{
                {"task", task.name},
                {"agent", agent.name},
                {"priority", priority},
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
