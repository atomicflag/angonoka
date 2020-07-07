#include "config/load.h"
#include "exceptions.h"
#include <catch2/catch.hpp>

#define ANGONOKA_COMMON_YAML                                         \
    "tasks:\n"                                                       \
    "  task1:\n"                                                     \
    "    duration:\n"                                                \
    "      min: 1 day\n"                                             \
    "      max: 3 days\n"

TEST_CASE("Loading agents")
{
    SECTION("No 'agents' section")
    {
        constexpr auto text = ANGONOKA_COMMON_YAML;
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Section 'agents' has an invalid type")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents: 123";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Invalid agent spec")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1: 123";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Invalid group spec")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    groups: 123";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Extra attributes")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    asdf: 123";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Parse groups")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    groups:\n"
            "      - A\n"
            "      - B\n"
            "  agent 2:\n"
            "    groups:\n"
            "      - A\n"
            "      - C\n";
        // clang-format on
        const auto system = angonoka::load_text(text);
        REQUIRE(system.groups == angonoka::Groups{"A", "B", "C"});
        REQUIRE(system.agents.size() == 2);
        // Agent 1 has A(0) and B(1)
        REQUIRE(
            system.agents[0].group_ids == angonoka::GroupIds{0, 1});
        // Agent 2 has A(0) and C(2)
        REQUIRE(
            system.agents[1].group_ids == angonoka::GroupIds{0, 2});
    }

    SECTION("Universal agents")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    groups:\n"
            "      - A\n"
            "      - B\n"
            "  agent 2:";
        // clang-format on
        const auto system = angonoka::load_text(text);
        // Agent 1 has A(0) and B(1)
        REQUIRE(
            system.agents[0].group_ids == angonoka::GroupIds{0, 1});
        // Agent 2 should be universal
        REQUIRE(system.agents[1].is_universal());
        REQUIRE(system.agents[1].can_work_on(0));
        REQUIRE(system.agents[1].can_work_on(1));
    }

    SECTION("No groups")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "  agent 2:";
        // clang-format on
        const auto system = angonoka::load_text(text);
        REQUIRE(system.agents[0].group_ids.empty());
        REQUIRE(system.agents[1].group_ids.empty());
    }

    SECTION("Invalid performance section")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Missing performance value")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:\n"
            "      min: 1.0";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Invalid performance type, text")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:\n"
            "      min: text\n"
            "      max: text";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Invalid performance type, dict")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:\n"
            "      min: 1\n"
            "      max:\n"
            "        - 2";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Invalid performance values")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:\n"
            "      min: 2.0\n"
            "      max: 1.0";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Parse performance")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:\n"
            "      min: 0.8\n"
            "      max: 1.8\n"
            "  agent 2:";
        // clang-format on
        const auto system = angonoka::load_text(text);
        const auto& agent1_performance = system.agents[0].performance;
        REQUIRE(agent1_performance.min == Approx(.8f));
        REQUIRE(agent1_performance.max == Approx(1.8f));
        const auto& agent2_performance = system.agents[1].performance;
        REQUIRE(agent2_performance.min == Approx(.5f));
        REQUIRE(agent2_performance.max == Approx(1.5f));
    }

    SECTION("Duplicate agents")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "  agent 1:";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Duplicate agent sections")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "agents:\n"
            "  agent 2:";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Exact performance")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance: 1.0\n";
        // clang-format on
        const auto system = angonoka::load_text(text);
        const auto& agent_performance = system.agents[0].performance;
        REQUIRE(agent_performance.min == Approx(1.f));
        REQUIRE(agent_performance.max == Approx(1.f));
    }

    SECTION("Agent performance validation")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance: -1.0\n";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }
}

#undef ANGONOKA_COMMON_YAML
