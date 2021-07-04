#include "events.h"
#include <fmt/printf.h>

namespace {
using namespace angonoka;
/**
    Check if an event is the final one.

    @param evt Event

    @return True if this is the final event.
*/
bool is_final_event(ProgressEvent& evt) noexcept
{
    using boost::variant2::get_if;
    if (auto* e = get_if<SimpleProgressEvent>(&evt))
        return *e == SimpleProgressEvent::Finished;
    return false;
}
} // namespace

namespace angonoka::cli {
void EventHandler::operator()(const SimpleProgressEvent& e) const
{
    switch (e) {
    case SimpleProgressEvent::ScheduleOptimizationStart:
        fmt::print("Optimizing the schedule...\n");
        start(*progress);
        return;
    case SimpleProgressEvent::Finished:
        fmt::print("Probability estimation complete.\n");
        return;
    }
}

void EventHandler::operator()(
    const ScheduleOptimizationEvent& e) const
{
    Expects(e.progress >= 0.F && e.progress <= 1.F);

    update(*progress, e.progress, "Optimization progress");
}

void EventHandler::operator()(
    const ScheduleOptimizationComplete& /* e */) const
{
    stop(*progress);
    if (options->color) {
        cursor_up();
        fmt::print("Optimizing the schedule... OK\n");
    } else {
        fmt::print("Schedule optimization complete.\n");
    }
    // TODO: Print optimization results
}

void consume_events(
    Queue<ProgressEvent>& queue,
    std::future<Prediction>& prediction,
    EventHandler handler)
{

    using namespace std::literals::chrono_literals;
    using boost::variant2::visit;
    constexpr auto event_timeout = 100ms;
    ProgressEvent evt;
    while (!is_final_event(evt)) {
        if (!queue.try_dequeue(evt)) {
            prediction.wait_for(event_timeout);
            continue;
        }
        visit(handler, evt);
    }
}
} // namespace angonoka::cli
