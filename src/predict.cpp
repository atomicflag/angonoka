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
#endif // UNIT_TEST

namespace {
using namespace angonoka;

constexpr auto event_queue_size = 100;

/**
    Convert normalized makespan to seconds.

    @param optimizer    Instance of Optimizer
    @param params       Schedule parameters

    @return Makespan in seconds
*/
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
OptimizedSchedule optimize(
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

    // TODO: Expose these in the Configuration
    // as well as beta_scale, stun_window,
    // gamma and restart_period
    // Maximum of 50 idle batches
    constexpr auto batch_size = 30'000;
    constexpr auto max_idle_iters = batch_size * 50;
    constexpr auto beta_scale = 1e-4F;
    constexpr auto stun_window = 10000;
    constexpr auto gamma = .5F;
    constexpr auto restart_period = 1 << 20;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbraced-scalar-init"
    stun::Optimizer optimizer{
        {.params{&params},
         .batch_size{batch_size},
         .max_idle_iters{max_idle_iters},
         .beta_scale{beta_scale},
         .stun_window{stun_window},
         .gamma{gamma},
         .restart_period{restart_period}}};
#pragma clang diagnostic pop
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

std::tuple<
    std::future<OptimizedSchedule>,
    std::shared_ptr<Queue<ProgressEvent>>>
schedule(const Configuration& config)
{
    auto events
        = std::make_shared<Queue<ProgressEvent>>(event_queue_size);
    auto future = std::async(std::launch::async, [events, &config] {
        events->enqueue(
            SimpleProgressEvent::ScheduleOptimizationStart);
        const auto schedule_params = stun::to_schedule_params(config);
        auto opt_result = optimize(schedule_params, *events);
        events->enqueue(ScheduleOptimizationComplete{
            .makespan{opt_result.makespan}});
        events->enqueue(SimpleProgressEvent::Finished);
        return opt_result;
    });
    return {std::move(future), std::move(events)};
}
} // namespace angonoka
