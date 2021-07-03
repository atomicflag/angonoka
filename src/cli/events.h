#pragma once

#include "options.h"
#include "predict.h"
#include "progress.h"
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
// TODO: doc, test, expects
struct EventHandler {
    gsl::not_null<Progress*> progress;
    gsl::not_null<const Options*> options;

    // TODO: doc, test, expects
    void operator()(const SimpleProgressEvent& e) const;

    // TODO: doc, test, expects
    void operator()(const ScheduleOptimizationEvent& e) const;

    // TODO: doc, test, expects
    void operator()(const ScheduleOptimizationComplete& e) const;
};

/**
    Check if an event is the final one.

    @param evt Event

    @return True if this is the final event.
*/
bool is_final_event(ProgressEvent& evt) noexcept;

// TODO: doc, test, expects
void consume_events(
    Queue<ProgressEvent>& queue,
    std::future<Prediction>& prediction,
    EventHandler&& handler);
} // namespace angonoka::cli
