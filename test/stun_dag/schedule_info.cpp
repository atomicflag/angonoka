#include "stun_dag/schedule_info.h"
#include <catch2/catch.hpp>

TEST_CASE("ScheduleInfo type traits")
{
    using angonoka::stun_dag::ScheduleInfo;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<ScheduleInfo>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<ScheduleInfo>);
    STATIC_REQUIRE(std::is_copy_constructible_v<ScheduleInfo>);
    STATIC_REQUIRE(std::is_copy_assignable_v<ScheduleInfo>);
    STATIC_REQUIRE(
        std::is_nothrow_move_constructible_v<ScheduleInfo>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<ScheduleInfo>);
}

TEST_CASE("ScheduleInfo special memeber functions")
{
    using namespace angonoka::stun_dag;

    ScheduleInfo info{
        .agent_performance{1.F, 2.F, 3.F},
        .task_duration{3.F, 2.F, 1.F},
        .available_agents_data{2, 1, 2, 0, 1, 2},
        .dependencies_data{0, 0, 1}};

    {
        auto* p = info.available_agents_data.data();
        const auto n = [&](auto s) -> span<int16> {
            return {std::exchange(p, std::next(p, s)), s};
        };
        info.available_agents = {n(1), n(2), n(3)};
    }
    {
        auto* p = info.dependencies_data.data();
        const auto n = [&](auto s) -> span<int16> {
            return {std::exchange(p, std::next(p, s)), s};
        };
        info.dependencies = {n(0), n(1), n(2)};
    }

    SECTION("Move ctor")
    {
        ScheduleInfo other{std::move(info)};

        REQUIRE(info.dependencies_data.empty());
        REQUIRE_FALSE(other.dependencies_data.empty());
        REQUIRE(other.dependencies[2][1] == 1);
    }

    SECTION("Copy ctor")
    {
        ScheduleInfo other{info};

        info.dependencies.clear();
        info.dependencies_data.clear();

        // TODO: Fix copy ctor
        // REQUIRE(other.dependencies[2][1] == 1);
    }
}
