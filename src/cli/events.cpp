#include "events.h"
#include "humanize.h"
#include "verbose.h"
#include <fmt/printf.h>

namespace angonoka::cli {
namespace detail {
    bool is_final_event(ProgressEvent& evt) noexcept
    {
        using boost::variant2::get_if;
        if (auto* e = get_if<SimpleProgressEvent>(&evt))
            return *e == SimpleProgressEvent::Finished;
        return false;
    }
} // namespace detail

void EventHandler::operator()(const SimpleProgressEvent& e)
{
    switch (e) {
    case SimpleProgressEvent::ScheduleOptimizationStart:
        fmt::print("Optimizing the schedule...\n");
        start(*progress);
        if (options->log_optimization) {
            opt_log.emplace(fmt::output_file("optimization_log.csv"));
            opt_log->print("progress,makespan,current_epoch\n");
        }
        return;
    default: return;
    }
}

void EventHandler::operator()(const ScheduleOptimizationEvent& e)
{
    Expects(e.progress >= 0.F && e.progress <= 1.F);

    update(*progress, e.progress, "Optimization progress");
    if (options->log_optimization) {
        opt_log->print(
            "{},{},{}\n",
            e.progress,
            e.makespan.count(),
            base_value(e.current_epoch));
    }
}

void EventHandler::operator()(
    const ScheduleOptimizationComplete& e) const
{
    stop(*progress);
    if (options->color) {
        cursor_up();
        fmt::print("Optimizing the schedule... OK\n");
    } else {
        fmt::print("Schedule optimization complete.\n");
    }
    constexpr auto text = "Optimal makespan: {}.\n";
    if (options->verbose) {
        fmt::print(text, verbose{e.makespan});
    } else {
        fmt::print(text, humanize{e.makespan});
    }
}
} // namespace angonoka::cli
