#include "schedule.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

namespace angonoka::detail {
using ranges::to;
using ranges::views::transform;

constexpr auto get_durations = transform([](const Task& t) {
    const auto& d = t.duration;
    Expects(d.min.count() > 0);
    Expects(d.max.count() > 0);

    return (d.min.count() + d.max.count()) / 2;
});

constexpr auto get_performance = transform([](const Agent& a) {
    const auto& perf = a.performance;
    Expects(perf.min > 0.F);
    Expects(perf.max > 0.F);

    return (perf.min + perf.max) / 2;
});

Constraints::Constraints(const System& sys)
    : durations{sys.tasks | get_durations | to<ExpectedDurations>()}
    , performance{sys.agents | get_performance | to<ExpectedPerformance>()}
    , agent_groups{sys.agents.size() * sys.tasks.size()}
{
    Expects(!sys.tasks.empty());
    Expects(!sys.agents.empty());

    const gsl::index agent_count = sys.agents.size();
    const gsl::index task_count = sys.tasks.size();
    for (gsl::index a = 0; a < agent_count; ++a) {
        for (gsl::index t = 0; t < task_count; ++t) {
            const auto i = a * task_count + t;
            const auto& task = sys.tasks[t];
            if (task.group_id) {
                agent_groups[i]
                    = sys.agents[a].can_work_on(*task.group_id);
            } else {
                agent_groups[i] = true;
            }
        }
    }

    Ensures(durations.size() == sys.tasks.size());
    Ensures(performance.size() == sys.agents.size());
    Ensures(
        agent_groups.size() == sys.tasks.size() * sys.agents.size());
}

bool Constraints::can_work_on(
    std::int_fast8_t agent_id,
    std::int_fast8_t task_id) const
{
    Expects(!agent_groups.empty());
    Expects(!durations.empty());
    Expects(static_cast<gsl::index>(task_id) < durations.size());
    Expects(static_cast<gsl::index>(agent_id) < performance.size());

    const gsl::index i
        = durations.size() * static_cast<gsl::index>(agent_id)
        + static_cast<gsl::index>(task_id);
    return agent_groups[i];
}

std::int_fast32_t makespan(
    IndividualView i,
    const Constraints& con,
    gsl::span<std::int_fast32_t> buf)
{
    Expects(!i.empty());
    Expects(!con.durations.empty());
    Expects(!con.performance.empty());
    Expects(i.size() == con.durations.size());
    Expects(buf.size() == con.performance.size());

    const auto& dur = con.durations;
    const auto& perf = con.performance;
    const auto size = i.size();
    const auto asize = perf.size();
    ranges::fill(buf, 0);

    for (gsl::index t{0}; t < size; ++t) {
        // NOLINTNEXTLINE(bugprone-signed-char-misuse)
        const auto ai = static_cast<gsl::index>(i[t]);
        buf[ai] += dur[t];
    }
    float result{0.F};
    for (gsl::index i{0}; i < asize; ++i) {
        const auto v = gsl::narrow_cast<float>(buf[i]) * perf[i];
        if (v > result) result = v;
    }

    Ensures(result > 0.F);

    return static_cast<std::int_fast32_t>(result);
}

void crossover(
    Parents /* p */,
    Individual /* i */,
    RandomEngine& /* gen */)
{
    // TODO: WIP
}
} // namespace angonoka::detail
