#include "schedule.h"
#include <gsl/gsl-lite.hpp>
#include <random>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
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

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Constraints::can_work_on(
    std::int_fast8_t agent_id,
    std::int_fast8_t task_id) const noexcept
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

// NOLINTNEXTLINE(bugprone-exception-escape)
float makespan(IndividualView i, const Constraints& con) noexcept
{
    Expects(!i.empty());
    Expects(!con.durations.empty());
    Expects(!con.performance.empty());
    Expects(i.size() == con.durations.size());

    thread_local Vector<float, static_alloc_agents> buf;

    const auto& dur = con.durations;
    const auto& perf = con.performance;
    const auto size = i.size();
    buf.resize(perf.size());
    ranges::fill(buf, 0.F);

    for (gsl::index t{0}; t < size; ++t) {
        // NOLINTNEXTLINE(bugprone-signed-char-misuse)
        const auto ai = static_cast<gsl::index>(i[t]);
        auto& v = buf[ai];
        v = std::fma(static_cast<float>(dur[t]), perf[ai], v);
    }
    return ranges::max(buf);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void crossover(Parents p, Individual i, RandomEngine& gen) noexcept
{
    Expects(!i.empty());
    using ParentDist = std::uniform_int_distribution<gsl::index>;
    thread_local ParentDist pd{0, number_of_parents - 1};
    const auto individual_size = i.size();
    for (gsl::index j{0}; j < individual_size; ++j)
        i[j] = p[pd(gen)][j];
}
} // namespace angonoka::detail
