#include "stun/task_agents.h"
#include "utils.h"
#include <catch2/catch.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>
#include <vector>

TEST_CASE("TaskAgents type traits")
{
    using angonoka::stun::TaskAgents;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<TaskAgents>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<TaskAgents>);
    STATIC_REQUIRE(std::is_copy_constructible_v<TaskAgents>);
    STATIC_REQUIRE(std::is_copy_assignable_v<TaskAgents>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<TaskAgents>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<TaskAgents>);
}

TEST_CASE("TaskAgents values")
{
    using namespace angonoka::stun;
    using angonoka::utils::make_array;
    using ranges::equal;
    using ranges::to;
    using ranges::views::chunk;

    constexpr auto data
        = make_array<int16>(0, 1, 2, 3, 4, 5, 6, 7, 8);
    const auto spans
        = data | chunk(3) | to<std::vector<span<const int16>>>();

    const TaskAgents task_agents{spans};

    REQUIRE(equal(task_agents[0], make_array<const int16>(0, 1, 2)));
    REQUIRE(equal(task_agents[1], make_array<const int16>(3, 4, 5)));
    REQUIRE(equal(task_agents[2], make_array<const int16>(6, 7, 8)));
}

TEST_CASE("TaskAgents copy constructor")
{
    using namespace angonoka::stun;
    using angonoka::utils::make_array;
    using ranges::equal;
    using ranges::to;
    using ranges::views::chunk;

    TaskAgents task_agents;

    {
        constexpr auto data = make_array<int16>(0, 1, 2);
        const auto spans
            = data | chunk(3) | to<std::vector<span<const int16>>>();

        const TaskAgents other_task_agents{spans};
        task_agents = other_task_agents;
    }

    REQUIRE(equal(task_agents[0], make_array<const int16>(0, 1, 2)));
}
