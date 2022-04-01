#include "predict.h"
#include "config/load.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/to_container.hpp>
#ifndef UNIT_TEST
#include "simulation.h"
#include "stun/optimizer.h"
#include "stun/schedule_params.h"
#else // UNIT_TEST
#include "stub/optimizer.h"
#include "stub/schedule_params.h"
#include "stub/simulation.h"
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
    const OptimizationParameters& opt_params,
    Queue<ProgressEvent>& events)
{
    Expects(!params.agent_performance.empty());
    Expects(!params.task_duration.empty());
    Expects(!params.available_agents.empty());
    Expects(
        params.available_agents.size()
        == params.task_duration.size());

    using namespace angonoka::stun;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbraced-scalar-init"
    stun::Optimizer optimizer{
        {.params{&params},
         .batch_size{opt_params.batch_size},
         .max_idle_iters{opt_params.max_idle_iters},
         .beta_scale{opt_params.beta_scale},
         .stun_window{opt_params.stun_window},
         .gamma{opt_params.gamma},
         .restart_period{opt_params.restart_period}}};
#pragma clang diagnostic pop
    while (!optimizer.has_converged()) {
        optimizer.update();
        events.enqueue(ScheduleOptimizationEvent{
            .progress = optimizer.estimated_progress(),
            .makespan = makespan(optimizer, params),
            .current_epoch = optimizer.current_epoch()});
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
        const auto opt_result
            = optimize(schedule_params, config.opt_params, *events);
        events->enqueue(ScheduleOptimizationComplete{
            .makespan{opt_result.makespan}});
        events->enqueue(SimpleProgressEvent::SimulationStart);
        __builtin_debugtrap();
        auto hist = histogram(config, opt_result); // TODO: coredumps here
        __builtin_debugtrap();
        auto hist_stats = stats(hist);

        events->enqueue(SimpleProgressEvent::Finished);
        return Prediction{std::move(hist), hist_stats};
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
        auto opt_result
            = optimize(schedule_params, config.opt_params, *events);
        events->enqueue(ScheduleOptimizationComplete{
            .makespan{opt_result.makespan}});
        events->enqueue(SimpleProgressEvent::Finished);
        return opt_result;
    });
    return {std::move(future), std::move(events)};
}
} // namespace angonoka
