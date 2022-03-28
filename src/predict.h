#pragma once

#include "configuration.h"
#include "stun/schedule.h"
#include <boost/variant2/variant.hpp>
#include <chrono>
#include <future>
#include <memory>
#include <readerwriterqueue/readerwriterqueue.h>
#include <tuple>
#include "simulation.h"

namespace angonoka {
template <typename... Ts>
using variant = boost::variant2::variant<Ts...>;

// TODO: doc, test, expects
struct Prediction {
    Histogram histogram;
    HistogramStats stats;
};

/**
    The result of the schedule optimization process.

    @var schedule Optimized schedule
    @var makespan Makespan in seconds
*/
struct OptimizedSchedule {
    std::vector<stun::ScheduleItem> schedule;
    std::chrono::seconds makespan;
};

template <typename... Ts>
using Queue = moodycamel::ReaderWriterQueue<Ts...>;

/**
    Events without attributes.
*/
enum class SimpleProgressEvent {
    ScheduleOptimizationStart,
    SimulationStart,
    Finished
};

/**
    Emitted during the schedule optimization step.

    @var progress       Optimization progress, 0 to 1
    @var makespan       The best makespan so far
    @var current_epoch  Last optimization epoch
*/
struct ScheduleOptimizationEvent {
    float progress;
    std::chrono::seconds makespan;
    int32 current_epoch;
};

/**
    Emitted when the schedule optimization is complete.

    @var makespan The makespan after optimization
*/
struct ScheduleOptimizationComplete {
    std::chrono::seconds makespan;
};

/**
    Prediction progress event.
*/
using ProgressEvent = variant<
    SimpleProgressEvent,
    ScheduleOptimizationEvent,
    ScheduleOptimizationComplete>;

/**
    Predict likelihood of a given system configuration.

    The event queue provides a way to monitor the progress
    of the prediction function.

    // TODO: test

    @param config Tasks, agents and other data.

    @return Prediction future and the event queue.
*/
std::tuple<
    std::future<Prediction>,
    std::shared_ptr<Queue<ProgressEvent>>>
predict(const Configuration& config);

/**
    Make and optimize a schedule for a given configuration.

    @param config Tasks, agents and other data.

    @return An optimized schedule.
*/
std::tuple<
    std::future<OptimizedSchedule>,
    std::shared_ptr<Queue<ProgressEvent>>>
schedule(const Configuration& config);
} // namespace angonoka
