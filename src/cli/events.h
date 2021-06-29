#pragma once

#include "options.h"
#include "predict.h"
#include "progress.h"
#include <boost/hana/functional/overload.hpp>
#include <fmt/printf.h>
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
/**
    Handle simple progress events.

    @param progress Text or graphical progress bar
    @param options  CLI options

    @return SimpleProgressEvent message handler.
*/
inline auto
on_simple_progress_event(Progress& progress, const Options& options)
{
    return [&](const SimpleProgressEvent& e) mutable {
        switch (e) {
        case SimpleProgressEvent::ScheduleOptimizationStart:
            fmt::print("Optimizing the schedule...\n");
            start(progress);
            return;
        case SimpleProgressEvent::ScheduleOptimizationDone:
            stop(progress);
            if (options.color) {
                cursor_up();
                fmt::print("Optimizing the schedule... OK\n");
            } else {
                fmt::print("Schedule optimization complete.\n");
            }
            // TODO: Print optimization results
            return;
        case SimpleProgressEvent::Finished:
            fmt::print("Probability estimation complete.\n");
            return;
        }
    };
}

/**
    Handle schedule optimization events.

    @param progress Text or graphical progress bar

    @return ScheduleOptimizationEvent message handler.
*/
inline auto on_schedule_optimization_event(Progress& progress)
{
    return [&](const ScheduleOptimizationEvent& e) mutable {
        Expects(e.progress >= 0.F && e.progress <= 1.F);

        update(progress, e.progress, "Optimization progress");
    };
}

/**
    Check if an event is the final one.

    @param evt Event

    @return True if this is the final event.
*/
bool is_final_event(ProgressEvent& evt) noexcept;

/**
    Compose callbacks into an event consumer function.

    @return Event consumer function.
*/
template <typename... Ts>
auto make_event_consumer(Ts&&... callbacks) noexcept
{
    return [&... callbacks = std::forward<Ts>(callbacks)](
               Queue<ProgressEvent>& queue,
               std::future<Prediction>& prediction) mutable {
        using boost::hana::overload;
        using namespace std::literals::chrono_literals;
        using boost::variant2::visit;
        constexpr auto event_timeout = 100ms;
        ProgressEvent evt;
        while (!is_final_event(evt)) {
            if (!queue.try_dequeue(evt)) {
                prediction.wait_for(event_timeout);
                continue;
            }
            visit(overload(callbacks...), evt);
        }
    };
}
} // namespace angonoka::cli
