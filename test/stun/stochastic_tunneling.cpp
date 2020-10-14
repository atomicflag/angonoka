#include "stun/stochastic_tunneling.h"
#include "stun/makespan_estimator.h"
#include "stun/random_utils.h"
#include "stun/task_agents.h"
#include "stun/task_duration_cache.h"
#include <catch2/catch.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>
#include <vector>

TEST_CASE("Stochastic tunnleing")
{
    using namespace angonoka::stun;
    using ranges::to;
    using ranges::views::chunk;

    const std::vector<int16>
        task_agents_data{0, 1, 0, 1};
    const auto spans = task_agents_data | chunk(2)
        | to<std::vector<span<const int16>>>();

    const TaskAgents task_agents{spans};

    const std::vector<float> task_duration_data{1.F, 2.F};
    const TaskDurationCache cache{
        task_duration_data,
        task_duration_data};

    MakespanEstimator estimator{2, &cache};

    RandomUtils random_utils{&task_agents};

    std::vector<int16> best_state{0, 0};
    // TODO: takes too much time
    // TODO: push diagnostics
    const auto result = stochastic_tunneling(
        random_utils,
        estimator,
        best_state,
        Alpha{.5F},
        Beta{1.F},
        BetaScale{.3F});

    REQUIRE(result.lowest_e == Approx(1.F));
}
