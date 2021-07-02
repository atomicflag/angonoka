#include "events.h"
#include <fmt/printf.h>

namespace angonoka::cli {
bool is_final_event(ProgressEvent& evt) noexcept
{
    using boost::variant2::get_if;
    if (auto* e = get_if<SimpleProgressEvent>(&evt))
        return *e == SimpleProgressEvent::Finished;
    return false;
}

void OnSimpleProgressEvent::operator()(
    const SimpleProgressEvent& e) const
{
    switch (e) {
    case SimpleProgressEvent::ScheduleOptimizationStart:
        fmt::print("Optimizing the schedule...\n");
        start(*progress);
        return;
    case SimpleProgressEvent::ScheduleOptimizationDone:
        stop(*progress);
        if (options->color) {
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
}

void OnScheduleOptimizationEvent::operator()(
    const ScheduleOptimizationEvent& e) const
{
    Expects(e.progress >= 0.F && e.progress <= 1.F);

    update(*progress, e.progress, "Optimization progress");
}
} // namespace angonoka::cli
