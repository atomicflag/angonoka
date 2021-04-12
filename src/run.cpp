#include "run.h"
#include "config/load.h"
#include "stun/optimizer.h"
#include <fmt/ranges.h>
#include <indicators/progress_bar.hpp>
#include <indicators/terminal_size.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/transform.hpp>
#include <utility>
#include <vector>

namespace angonoka {
/**
    Find the optimal schedule.

    TODO: test, expects

    @param params Instance of ScheduleParams

    @return Optimal schedule.
*/
std::vector<stun::StateItem>
optimize(const stun::ScheduleParams& params)
{
    Expects(!params.agent_performance.empty());
    Expects(!params.task_duration.empty());
    Expects(!params.available_agents.empty());
    Expects(
        params.available_agents.size()
        == params.task_duration.size());

    using namespace angonoka::stun;

    constexpr auto batch_size = 10'000;
    constexpr auto max_idle_iters = 1'000'000;

    Optimizer optimizer{
        params,
        BatchSize{batch_size},
        MaxIdleIters{max_idle_iters}};
    while (!optimizer.has_converged()) {
        optimizer.update();
        fmt::print("{}\n", optimizer.estimated_progress());
    }

    // TODO: track progress via progress bar
    // TODO: return the Result, not just the state
    // Add a new class that encapsulates stun and adds
    // stopping condition and has an update method.
    return ranges::to<std::vector<StateItem>>(optimizer.state());
}
#pragma clang diagnostic pop

void run(std::string_view tasks_yml)
{
    using namespace angonoka::stun;

    // TODO: Handle YAML exceptions
    const auto config = load_file(tasks_yml);
    const auto schedule_params = to_schedule_params(config);
    const auto state = optimize(schedule_params);
    fmt::print("{}\n", state);
}
} // namespace angonoka
