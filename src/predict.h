#pragma once

#include "configuration.h"
#include "stun/schedule.h"
#include <boost/histogram.hpp>
#include <boost/variant2/variant.hpp>
#include <chrono>
#include <future>
#include <memory>
#include <readerwriterqueue/readerwriterqueue.h>
#include <tuple>

namespace angonoka {
template <typename... Ts>
using variant = boost::variant2::variant<Ts...>;

/**
    Histogram type alias.

    Used as a return type of the histogram function.

    The size of the bins is picked dynamically according to
    the expected makespan. Each bin contains the count of
    simulations within the bin's makespan range.
*/
using Histogram = boost::histogram::histogram<
    std::tuple<boost::histogram::axis::regular<
        float,
        boost::use_default,
        boost::histogram::axis::null_type,
        boost::histogram::axis::option::growth_t>>>;

/**
    Histogram percentiles.
*/
struct HistogramStats {
    std::chrono::seconds p25;
    std::chrono::seconds p50;
    std::chrono::seconds p75;
    std::chrono::seconds p95;
    std::chrono::seconds p99;
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

/**
    The result of running the simulations.

    Angonoka uses statistical modeling to run the simulations
    to figure out the most likely durations for tasks. The
    result contains the histogram of simulation runs and
    quantiles for various probabilities.

    For example stats.p95 will contain the total duration
    in seconds for the 95% quantile.

    @var histogram  Histogram for the simulation
    @var stats      Quantiles for the histogram
    @var schedule   Optimized schedule
    @var makespan   Makespan in seconds
*/
struct Prediction {
    Histogram histogram;
    HistogramStats stats;
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
