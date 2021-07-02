#pragma once

#include "options.h"
#include "predict.h"
#include "progress.h"
#include <boost/hana/functional/overload.hpp>
#include <gsl/gsl-lite.hpp>

namespace angonoka::cli {
/**
    Handle simple progress events.

    @var progress Text or graphical progress bar
    @var options  CLI options
*/
struct OnSimpleProgressEvent {
    gsl::not_null<Progress*> progress;
    gsl::not_null<const Options*> options;

    /**
        Handler.

        @param e Event
    */
    void operator()(const SimpleProgressEvent& e) const;
};

/**
    Handle schedule optimization events.

    @var progress Text or graphical progress bar
*/
struct OnScheduleOptimizationEvent {
    gsl::not_null<Progress*> progress;

    /**
        Handler.

        @param e Event
    */
    void operator()(const ScheduleOptimizationEvent& e) const;
};

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
    return [... callbacks = std::forward<Ts>(callbacks)](
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
