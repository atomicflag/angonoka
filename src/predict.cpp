#include "predict.h"
#include "config/load.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/to_container.hpp>
#ifndef UNIT_TEST
#include "stun/optimizer.h"
#include "stun/schedule_params.h"
#else // UNIT_TEST
#include "stub/optimizer.h"
#include "stub/schedule_params.h"
#endif

namespace {
using namespace angonoka;

// TODO: doc, test, expects
std::vector<stun::StateItem> optimize(
    const stun::ScheduleParams& params,
    Queue<ProgressEvent>& events)
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

    stun::Optimizer optimizer{
        params,
        BatchSize{batch_size},
        MaxIdleIters{max_idle_iters}};
    while (!optimizer.has_converged()) {
        optimizer.update();
        events.enqueue(ScheduleOptimizationEvent{
            .progress = optimizer.estimated_progress()});
    }

    return ranges::to<std::vector<StateItem>>(optimizer.state());
}
} // namespace
namespace angonoka {
std::tuple<
    std::future<Prediction>,
    std::shared_ptr<Queue<ProgressEvent>>>
predict(const Configuration& config)
{
    constexpr auto event_queue_size = 100;
    auto events
        = std::make_shared<Queue<ProgressEvent>>(event_queue_size);
    auto future = std::async(std::launch::async, [=] {
        events->enqueue(SimpleProgressEvent::Start);
        // TODO: stub to_schedule, etc
        const auto schedule_params = stun::to_schedule_params(config);
        const auto state = optimize(schedule_params, *events);
        events->enqueue(SimpleProgressEvent::Done);
        return Prediction{};
    });
    return {std::move(future), std::move(events)};
}
} // namespace angonoka
