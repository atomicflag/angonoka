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
struct OptimizationResult {
    std::vector<stun::ScheduleItem> schedule;
    std::chrono::seconds makespan;
};

// TODO: doc, test, expects
std::chrono::seconds makespan(
    const stun::Optimizer& optimizer,
    const stun::ScheduleParams& params)
{

    using seconds = std::chrono::seconds::rep;
    const auto duration = gsl::narrow<seconds>(std::trunc(
        optimizer.normalized_makespan()
        * params.duration_multiplier));
    return std::chrono::seconds{duration};
}

/**
    Find the optimal schedule.

    @param params Schedule parameters
    @param event Event queue

    @return Optimal schedule
*/
OptimizationResult optimize(
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
            .progress = optimizer.estimated_progress(),
            .makespan = makespan(optimizer, params)});
    }

    return {
        .schedule{ranges::to<std::vector<ScheduleItem>>(
            optimizer.schedule())},
        .makespan{makespan(optimizer, params)}};
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
    auto future = std::async(std::launch::async, [events, &config] {
        events->enqueue(
            SimpleProgressEvent::ScheduleOptimizationStart);
        const auto schedule_params = stun::to_schedule_params(config);
        const auto opt_result = optimize(schedule_params, *events);
        events->enqueue(ScheduleOptimizationComplete{
            .makespan{opt_result.makespan}});
        // TODO: WIP do other stuff here
        events->enqueue(SimpleProgressEvent::Finished);
        return Prediction{};
    });
    return {std::move(future), std::move(events)};
}
} // namespace angonoka
