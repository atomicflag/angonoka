#include "json_schedule.h"
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
namespace detail {
    nlohmann::json to_json(
        const Configuration& /* config */,
        const OptimizedSchedule& /* schedule */)
    {
        // TODO: WIP: Implement
        return {};
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
