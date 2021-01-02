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
        .task_duration{3.F, 2.F, 1.F}};

    {
        std::vector<int16> available_agents_data{2, 1, 2, 0, 1, 2};
        auto* p = available_agents_data.data();
        const auto n = [&](auto s) -> span<int16> {
            return {std::exchange(p, std::next(p, s)), s};
        };
        std::vector<span<int16>> available_agents
            = {n(1), n(2), n(3)};
        info.available_agents
            = {std::move(available_agents_data),
               std::move(available_agents)};
    }
    {
        std::vector<int16> dependencies_data{0, 0, 1};
        auto* p = dependencies_data.data();
        const auto n = [&](auto s) -> span<int16> {
            return {std::exchange(p, std::next(p, s)), s};
        };
        std::vector<span<int16>> dependencies = {n(0), n(1), n(2)};
        info.dependencies
            = {std::move(dependencies_data), std::move(dependencies)};
    }

    SECTION("Move ctor")
    {
        ScheduleInfo other{std::move(info)};

        REQUIRE(info.dependencies.empty());
        REQUIRE_FALSE(other.dependencies.empty());
        REQUIRE(other.dependencies[2u][1] == 1);
    }

    SECTION("Move assignment")
    {
        ScheduleInfo other;
        other = std::move(info);

        REQUIRE(info.dependencies.empty());
        REQUIRE_FALSE(other.dependencies.empty());
        REQUIRE(other.dependencies[2u][1] == 1);
    }

    SECTION("Copy ctor")
    {
        ScheduleInfo other{info};

        info.dependencies.clear();

        REQUIRE(other.dependencies[2u][1] == 1);
    }

    SECTION("Copy assignment")
    {
        ScheduleInfo other;
        other = info;

        info.dependencies.clear();

        REQUIRE(other.dependencies[2u][1] == 1);
    }
}

TEST_CASE("VectorOfSpans type traits")
{
    using angonoka::stun_dag::VectorOfSpans;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<VectorOfSpans>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<VectorOfSpans>);
    STATIC_REQUIRE(std::is_copy_constructible_v<VectorOfSpans>);
    STATIC_REQUIRE(std::is_copy_assignable_v<VectorOfSpans>);
    STATIC_REQUIRE(
        std::is_nothrow_move_constructible_v<VectorOfSpans>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<VectorOfSpans>);
}

TEST_CASE("VectorOfSpans special memeber functions")
{
    using namespace angonoka::stun_dag;

    SECTION("Empty")
    {
        VectorOfSpans vspans;

        REQUIRE(vspans.empty());

        SECTION("Copy ctor")
        {
            VectorOfSpans other{vspans};

            REQUIRE(other.empty());
        }
    }

    SECTION("Non-empty")
    {

        std::vector<int16> data{0, 1, 2};
        auto* b = data.data();
        const auto f = [&](auto s) -> span<int16> {
            return {std::exchange(b, std::next(b, s)), s};
        };
        std::vector<span<int16>> spans{f(1), f(1), f(1)};

        VectorOfSpans vspans{std::move(data), std::move(spans)};

        REQUIRE(vspans.size() == 3);

        SECTION("Copy ctor")
        {
            VectorOfSpans other{vspans};
            vspans.clear();

            REQUIRE(other.size() == 3);
            REQUIRE(other[2u][0] == 2);
        }

        SECTION("Copy assignment")
        {
            VectorOfSpans other;
            other = vspans;
            vspans.clear();

            REQUIRE(other.size() == 3);
            REQUIRE(other[2u][0] == 2);
        }

        SECTION("Move ctor")
        {
            VectorOfSpans other{std::move(vspans)};

            REQUIRE(vspans.empty());
            REQUIRE(other.size() == 3);
            REQUIRE(other[2u][0] == 2);
        }

        SECTION("Move assignment")
        {
            VectorOfSpans other;
            other = std::move(vspans);

            REQUIRE(vspans.empty());
            REQUIRE(other.size() == 3);
            REQUIRE(other[2u][0] == 2);
        }
    }
}
