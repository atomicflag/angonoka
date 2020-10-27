#include "stun/stochastic_tunneling.h"
#include "stun/makespan_estimator.h"
#include "stun/random_utils.h"
#include "stun/task_agents.h"
#include "stun/task_duration_cache.h"
#include "utils.h"
#include <celero/Celero.h>
#include <chrono>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/linear_distribute.hpp>
#include <range/v3/view/take.hpp>
#include <thread>
#include <vector>

#include <fmt/printf.h>

namespace {
using namespace angonoka::stun;
using ranges::to;
using ranges::views::chunk;
using ranges::views::cycle;
using ranges::views::iota;
using ranges::views::linear_distribute;
using ranges::views::take;

struct STUNFixture : celero::TestFixture {
    static constexpr auto agent_count = 10;
    static constexpr auto task_count = agent_count * 5;

    std::vector<int16> agent_indices = iota(0, agent_count) | cycle
        | take(task_count * agent_count) | to<std::vector<int16>>();
    std::vector<span<const int16>> spans = agent_indices
        | chunk(agent_count) | to<std::vector<span<const int16>>>();
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    std::vector<float> task_durations
        = linear_distribute(.1F / 2.6F, .9F / 2.6F, 5) | cycle
        | take(task_count) | to<std::vector<float>>();
    std::vector<float> agent_performances
        = std::vector(agent_count, 1.F);

    TaskAgents task_agents{spans};
    // static constexpr auto random_engine_seed = 123;
    RandomUtils random_utils{&task_agents};
    TaskDurationCache task_duration{
        task_durations,
        agent_performances};
    MakespanEstimator makespan{agent_count, &task_duration};
};
} // namespace

// NOLINTNEXTLINE(readability-redundant-member-init)
BASELINE_F(DemoSimple, Baseline, STUNFixture, 0, 0)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
    const std::vector<int16> best_state(task_count, 0);
    const auto result = stochastic_tunneling(
        random_utils,
        makespan,
        best_state,
        Gamma{.5F},
        Beta{1.F},
        BetaScale{.3F});
    celero::DoNotOptimizeAway(result);
    // TODO: percentile
    // fmt::print("{}\n", result.energy);
    // for(const auto& v : result.state)
    //     fmt::print("{} ", v);
    // fmt::print("\n");
#pragma clang diagnostic pop
}
