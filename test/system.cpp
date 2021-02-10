#include <catch2/catch.hpp>
#include <type_traits>

#include "configuration.h"

TEST_CASE("Configuration type traits")
{
    using angonoka::Configuration;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Configuration>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<Configuration>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Configuration>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Configuration>);
    STATIC_REQUIRE(std::is_move_constructible_v<Configuration>);
    STATIC_REQUIRE(std::is_move_assignable_v<Configuration>);
}

TEST_CASE("Agent type traits")
{
    using angonoka::Agent;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Agent>);
    STATIC_REQUIRE(std::is_default_constructible_v<Agent>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Agent>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Agent>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Agent>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Agent>);
}

TEST_CASE("Task type traits")
{
    using angonoka::Task;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Task>);
    STATIC_REQUIRE(std::is_default_constructible_v<Task>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Task>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Task>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Task>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Task>);
}

TEST_CASE("Configuration utility functions")
{
    angonoka::Configuration s;

    s.groups.emplace_back("Test Group");
    auto& t = s.tasks.emplace_back();
    auto& a = s.agents.emplace_back();

    REQUIRE(has_universal_agents(s));
    REQUIRE(can_work_on(a, t));

    a.group_ids.emplace(angonoka::GroupIndex{0});

    REQUIRE_FALSE(has_universal_agents(s));
    REQUIRE_FALSE(can_work_on(a, t));

    t.group_id = angonoka::GroupIndex{0};

    REQUIRE(can_work_on(a, t));
}

TEST_CASE("Agent utility functions")
{
    angonoka::Agent a;

    REQUIRE(is_universal(a));
    REQUIRE(can_work_on(a, angonoka::GroupIndex{0}));

    a.group_ids.emplace(angonoka::GroupIndex{1});

    REQUIRE_FALSE(is_universal(a));
    REQUIRE_FALSE(can_work_on(a, angonoka::GroupIndex{0}));
    REQUIRE(can_work_on(a, angonoka::GroupIndex{1}));
}

TEST_CASE("Agent member functions")
{
    angonoka::Agent a;

    REQUIRE(a.performance.average() == Approx(1.F));

    a.performance.min = 0.05F;
    a.performance.max = 0.15F;

    REQUIRE(a.performance.average() == Approx(.1F));
}

TEST_CASE("Task member functions")
{
    using namespace std::literals::chrono_literals;
    angonoka::Task t{.duration{.min{1s}, .max{3s}}};

    REQUIRE(t.duration.average() == 2s);
}
