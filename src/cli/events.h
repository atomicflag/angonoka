#pragma once

#include "options.h"
#include "predict.h"
#include "progress.h"
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
/**
    Prediction events handler.

    Prints various progress messages and results as
    the prediction algorithm runs.

    @var progress Chosen progress bar implementation
    @var options CLI options
*/
struct EventHandler {
    gsl::not_null<Progress*> progress;
    gsl::not_null<const Options*> options;

    /**
        Handler events without attributes.

        @param e Event
    */
    void operator()(const SimpleProgressEvent& e) const;

    /**
        Handle schedule optimization events.

        @param e Event
    */
    void operator()(const ScheduleOptimizationEvent& e) const;

    /**
        Handle schedule optimization completion.

        @param e Event
    */
    void operator()(const ScheduleOptimizationComplete& e) const;
};

/**
    Consumes prediction events from the queue.

    @param queue        Prediction events queue
    @param prediction   Prediction result
    @param handler      Events handler
*/
void consume_events(
    Queue<ProgressEvent>& queue,
    std::future<Prediction>& prediction,
    EventHandler handler);
} // namespace angonoka::cli
