#include "json_schedule.h"
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
namespace detail {
    nlohmann::json to_json(
        const Configuration& config,
        const OptimizedSchedule& schedule)
    {
        using nlohmann::json;
        // TODO: try range transform/to combo
        json tasks;

        for (const auto& t : schedule.schedule) {
            const auto& task
                = config.tasks[static_cast<std::size_t>(t.task_id)];
            tasks.emplace_back(json{{"name", task.name}});
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
