#include "configuration.h"
#include <doctest/doctest.h>
#include <type_traits>

TEST_CASE("Configuration")
{
    SUBCASE("configuration type traits")
    {
        using angonoka::Configuration;
        REQUIRE_UNARY(std::is_nothrow_destructible_v<Configuration>);
        REQUIRE_UNARY(
            std::is_nothrow_default_constructible_v<Configuration>);
        REQUIRE_UNARY(std::is_copy_constructible_v<Configuration>);
        REQUIRE_UNARY(std::is_copy_assignable_v<Configuration>);
        REQUIRE_UNARY(std::is_move_constructible_v<Configuration>);
        REQUIRE_UNARY(std::is_move_assignable_v<Configuration>);
    }

    SUBCASE("agent type traits")
    {
        using angonoka::Agent;
        REQUIRE_UNARY(std::is_nothrow_destructible_v<Agent>);
        REQUIRE_UNARY(std::is_default_constructible_v<Agent>);
        REQUIRE_UNARY(std::is_copy_constructible_v<Agent>);
        REQUIRE_UNARY(std::is_copy_assignable_v<Agent>);
        REQUIRE_UNARY(std::is_nothrow_move_constructible_v<Agent>);
        REQUIRE_UNARY(std::is_nothrow_move_assignable_v<Agent>);
    }

    SUBCASE("task type traits")
    {
        using angonoka::Task;
        REQUIRE_UNARY(std::is_nothrow_destructible_v<Task>);
        REQUIRE_UNARY(std::is_default_constructible_v<Task>);
        REQUIRE_UNARY(std::is_copy_constructible_v<Task>);
        REQUIRE_UNARY(std::is_copy_assignable_v<Task>);
        REQUIRE_UNARY(std::is_nothrow_move_constructible_v<Task>);
        REQUIRE_UNARY(std::is_nothrow_move_assignable_v<Task>);
    }

    SUBCASE("configuration utility functions")
    {
        angonoka::Configuration s;

        s.groups.emplace_back("Test Group");
        auto& t = s.tasks.emplace_back();
        auto& a = s.agents.emplace_back();

        REQUIRE_UNARY(has_universal_agents(s));
        REQUIRE_UNARY(can_work_on(a, t));

        a.group_ids.emplace(angonoka::GroupIndex{0});

        REQUIRE_UNARY_FALSE(has_universal_agents(s));
        REQUIRE_UNARY_FALSE(can_work_on(a, t));

        t.group_ids.emplace(angonoka::GroupIndex{0});

        REQUIRE_UNARY(can_work_on(a, t));
    }

    SUBCASE("agent utility functions")
    {
        angonoka::Agent a;

        REQUIRE_UNARY(is_universal(a));
        REQUIRE_UNARY(can_work_on(a, angonoka::GroupIndex{0}));

        a.group_ids.emplace(angonoka::GroupIndex{1});

        REQUIRE_UNARY_FALSE(is_universal(a));
        REQUIRE_UNARY_FALSE(can_work_on(a, angonoka::GroupIndex{0}));
        REQUIRE_UNARY(can_work_on(a, angonoka::GroupIndex{1}));
    }

    SUBCASE("agent member functions")
    {
        angonoka::Agent a;

        REQUIRE_EQ(a.performance.average(), doctest::Approx(1.F));

        a.performance.min = 0.05F;
        a.performance.max = 0.15F;

        REQUIRE_EQ(a.performance.average(), doctest::Approx(.1F));
    }

    SUBCASE("task member functions")
    {
        using namespace std::literals::chrono_literals;
        angonoka::Task t{.duration{.min{1s}, .max{3s}}};

        REQUIRE_EQ(t.duration.average(), 2s);
    }
}
