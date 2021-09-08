#include "json_schedule.h"
#include <gsl/gsl-lite.hpp>
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
    [[nodiscard]] float task_duration(
        const Configuration& config,
        int16 task_id,
        int16 agent_id)
    {
        Expects(task_id >= 0);
        Expects(agent_id >= 0);
        const auto& task
            = config.tasks[static_cast<gsl::index>(task_id)];
        const auto& agent
            = config.agents[static_cast<gsl::index>(agent_id)];
        return static_cast<float>(task.duration.average().count())
            * agent.performance.average();
    }

    nlohmann::json to_json(
        const Configuration& config,
        const OptimizedSchedule& schedule)
    {
        using nlohmann::json;
        json tasks;
        std::vector<int> agent_priority(config.agents.size());

        for (const auto& t : schedule.schedule) {
            const auto& task
                = config.tasks[static_cast<gsl::index>(t.task_id)];
            const auto& agent
                = config.agents[static_cast<gsl::index>(t.agent_id)];
            const auto duration
                = task_duration(config, t.task_id, t.agent_id);
            const auto priority
                = agent_priority[static_cast<gsl::index>(
                    t.agent_id)]++;

            // TODO: expected_start? same logic as Makespan?
            tasks.emplace_back(json{
                {"task", task.name},
                {"agent", agent.name},
                {"priority", priority},
                {"expected_duration", static_cast<int>(duration)}});
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
