#include "stun_dag/random_utils.h"
#include "stun_dag/schedule_info.h"
#include "stun_dag/utils.h"
#include <catch2/catch.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/view/chunk.hpp>

TEST_CASE("Mutate state")
{
    using namespace angonoka::stun_dag;

    ScheduleInfo info;
    info.agent_performance = {1.F, 2.F, 3.F};
    info.task_duration = {1.F, 2.F, 3.F};
    info.available_agents_data = {0, 1, 2};
    info.available_agents
        = {info.available_agents_data,
           info.available_agents_data,
           info.available_agents_data};
    info.dependencies = {{}, {}, {}};

    SECTION("No dependencies")
    {
        RandomUtils random{0};

        std::vector<StateItem> state{{0, 0}, {1, 1}, {2, 2}};

        mutate(state, info, random);

        REQUIRE(
            state == std::vector<StateItem>{{1, 0}, {0, 2}, {2, 2}});

        for (int i{0}; i < 100; ++i) mutate(state, info, random);

        REQUIRE(
            state == std::vector<StateItem>{{1, 0}, {2, 0}, {0, 2}});
    }

    SECTION("With dependencies")
    {
        using ranges::actions::push_back;
        using ranges::views::chunk;
        info.dependencies_data = {1, 2};
        info.dependencies_data = {0, 1};
        info.dependencies.clear();
        info.dependencies.emplace_back();
        push_back(
            info.dependencies,
            info.dependencies_data | chunk(1));

        RandomUtils random{0};

        std::vector<StateItem> state{{0, 0}, {1, 1}, {2, 2}};

        mutate(state, info, random);

        REQUIRE(
            state == std::vector<StateItem>{{0, 0}, {1, 2}, {2, 2}});

        for (int i{0}; i < 100; ++i) mutate(state, info, random);

        REQUIRE(
            state == std::vector<StateItem>{{0, 0}, {1, 0}, {2, 2}});
    }
}
