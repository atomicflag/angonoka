#include <catch2/catch.hpp>
#include <type_traits>

#include "src/system.h"

TEST_CASE("System type traits")
{
    using angonoka::System;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<System>);
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<System>);
    STATIC_REQUIRE(std::is_copy_constructible_v<System>);
    STATIC_REQUIRE(std::is_copy_assignable_v<System>);
    STATIC_REQUIRE(std::is_move_constructible_v<System>);
    STATIC_REQUIRE(std::is_move_assignable_v<System>);
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

TEST_CASE("System member functions")
{
    angonoka::System s;

    s.groups.emplace_back("Test Group");
    auto& a = s.agents.emplace_back();

    REQUIRE(has_universal_agents(s));

    a.group_ids.emplace(angonoka::GroupId{0});

    REQUIRE_FALSE(has_universal_agents(s));
}

TEST_CASE("Agent member functions")
{
    angonoka::Agent a;

    REQUIRE(is_universal(a));
    REQUIRE(can_work_on(a, angonoka::GroupId{0}));

    a.group_ids.emplace(angonoka::GroupId{1});

    REQUIRE_FALSE(is_universal(a));
    REQUIRE_FALSE(can_work_on(a, angonoka::GroupId{0}));
    REQUIRE(can_work_on(a, angonoka::GroupId{1}));
}
