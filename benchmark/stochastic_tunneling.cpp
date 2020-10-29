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

struct STUNState {
    gsl::index agent_count = 5;
    gsl::index tasks_per_agent = 5;
    gsl::index task_count = agent_count * tasks_per_agent;

    std::vector<int16> agent_indices = iota(0, static_cast<int>(agent_count)) | cycle
        | take(task_count * agent_count) | to<std::vector<int16>>();
    std::vector<span<const int16>> spans = agent_indices
        | chunk(agent_count) | to<std::vector<span<const int16>>>();
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    int duration_sum
        = (tasks_per_agent / 2.F) * (1.F / tasks_per_agent + 1.F);
    std::vector<float> task_durations
        = linear_distribute(
              1.F / tasks_per_agent / duration_sum,
              1.F / duration_sum,
              tasks_per_agent)
        | cycle | take(task_count) | to<std::vector<float>>();
    std::vector<float> agent_performances
        = std::vector(agent_count, 1.F);

    TaskAgents task_agents{spans};
    // static constexpr auto random_engine_seed = 123;
    RandomUtils random_utils{&task_agents};
    TaskDurationCache task_duration{
        task_durations,
        agent_performances};
    MakespanEstimator makespan{agent_count, &task_duration};
    std::vector<int16> best_state = std::vector<int16>(task_count, 0);
};

struct STUNFixture : celero::TestFixture {

    [[nodiscard]] std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        return {0, 1, 2};
    }

    void setUp(const celero::TestFixture::ExperimentValue&
                   val) override
    {
        const auto i = val.Value; 
        // TODO: grr
        state = std::make_unique<STUNState>(gsl::at(data, i)[0], gsl::at(data,i)[1]);
    }


    std::unique_ptr<STUNState> state;
    std::array<std::array<gsl::index, 2>, 3> data{
        {{5, 5}, {5, 10}, {10, 10}}};
};
} // namespace

// NOLINTNEXTLINE(readability-redundant-member-init)
BASELINE_F(StochasticTunneling, Small, STUNFixture, 0, 0)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
    const auto result = stochastic_tunneling(
        state.random_utils,
        state.makespan,
        state.best_state,
        Gamma{.5F},
        Beta{1.F},
        BetaScale{.3F});
    celero::DoNotOptimizeAway(result);
    fmt::print("{}\n", result.energy);
    for (const auto& v : result.state) fmt::print("{} ", v);
    fmt::print("\n");
#pragma clang diagnostic pop
}
