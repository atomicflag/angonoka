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
#include <range/v3/view/take.hpp>
#include <thread>
#include <vector>

namespace {
using namespace angonoka::stun;
using ranges::to;
using ranges::views::chunk;
using ranges::views::cycle;
using ranges::views::iota;
using ranges::views::take;

struct STUNFixture : celero::TestFixture {
    static constexpr auto agent_count = 3;
    static constexpr auto task_count = agent_count * 5;

    std::vector<int16> agent_indices = iota(0, 2) | cycle
        | take(task_count * agent_count) | to<std::vector<int16>>();
    std::vector<span<const int16>> spans = agent_indices
        | chunk(agent_count) | to<std::vector<span<const int16>>>();
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    std::vector<float> task_durations{.1F, .3F, .4F, .5F, .7F};
    std::vector<float> agent_performances = std::vector(agent_count, 1.F);

    TaskAgents task_agents{spans};
    static constexpr auto random_engine_seed = 123;
    RandomUtils random_utils{&task_agents, random_engine_seed};
    TaskDurationCache task_duration{
        task_durations,
        agent_performances};
    MakespanEstimator makespan{agent_count, &task_duration};
};
} // namespace

// NOLINTNEXTLINE(readability-redundant-member-init)
BASELINE_F(DemoSimple, Baseline, STUNFixture, 0, 0)
{
    std::this_thread::sleep_for(std::chrono::nanoseconds{1});
}
