#include <clipp.h>
#include <fmt/printf.h>
#include <memory>
#include <string>

#include <boost/safe_numerics/automatic.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#include <cstdint>
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <utility>

// makespan
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/view/transform.hpp>

namespace angonoka::stun2 {
namespace sn = boost::safe_numerics;
using ranges::span;
using int16 = sn::safe<std::int_fast16_t, sn::automatic>;
struct StateItem {
    int16 task_id;
    int16 agent_id;
};
using State = span<StateItem>;

struct ScheduleInfo {
    std::vector<float> agent_performance;
    std::vector<float> task_duration;

    std::vector<int16> available_agents_data;
    std::vector<span<int16>> available_agents;

    std::vector<int16> dependencies_data;
    std::vector<span<int16>> dependencies;
};

enum class TasksCount : int {};
enum class AgentsCount : int {};

// TODO: test
class Makespan {
public:
    Makespan(
        gsl::not_null<ScheduleInfo*> info,
        TasksCount tasks_count,
        AgentsCount agents_count)
        : info{std::move(info)}
        , sum_buffer(
              static_cast<gsl::index>(tasks_count)
              + static_cast<gsl::index>(agents_count))
        , task_done{sum_buffer.data(), static_cast<int>(tasks_count)}
        , work_done{task_done.end(), static_cast<int>(agents_count)}
    {
    }

    float operator()(State state) noexcept
    {
        ranges::fill(sum_buffer, 0.F);
        for (auto [task_id, agent_id] : state) {
            const auto done = std::max(
                                  dependency_done(task_id),
                                  work_done[agent_id])
                + task_duration(task_id, agent_id);
            work_done[agent_id] = task_done[task_id] = done;
        }
        return ranges::max(work_done);
    }

private:
    gsl::not_null<ScheduleInfo*> info;
    std::vector<float> sum_buffer;
    span<float> task_done;
    span<float> work_done;

    [[nodiscard]] float dependency_done(int16 task_id) const noexcept
    {
        using ranges::views::transform;
        const auto deps = info->dependencies[task_id];
        if (deps.empty()) return 0.F;
        return ranges::max(deps | transform([&](const auto& dep_id) {
                               return task_done[dep_id];
                           }));
    }

    [[nodiscard]] float
    task_duration(int16 task_id, int16 agent_id) const noexcept
    {

        return info->task_duration[task_id]
            * info->agent_performance[agent_id];
    }
};
} // namespace angonoka::stun2

namespace {
struct Options {
};
} // namespace

int main(int argc, char** argv)
{
    using namespace clipp;

    // Options options;

    // group cli{value("input file", options.filename)};
    //
    // if (!parse(argc, argv, cli)) {
    //     fmt::print("{}", make_man_page(cli, *argv));
    //     return 1;
    // }

    return 0;
}
