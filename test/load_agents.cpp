#include "config/load.h"
#include "exceptions.h"
#include <catch2/catch.hpp>

#define ANGONOKA_COMMON_YAML                                         \
    "tasks:\n"                                                       \
    "  task1:\n"                                                     \
    "    days:\n"                                                    \
    "      min: 1\n"                                                 \
    "      max: 3\n"

TEST_CASE("Loading agents")
{
    SECTION("No 'agents' section")
    {
        constexpr auto text = ANGONOKA_COMMON_YAML;
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::InvalidTasksDef);
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
            angonoka::InvalidTasksDef);
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
            angonoka::InvalidTasksDef);
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
            angonoka::InvalidTasksDef);
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
            angonoka::InvalidTasksDef);
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

    SECTION("Fill empty groups")
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
        // Agent 2 should have all groups
        REQUIRE(
            system.agents[1].group_ids == angonoka::GroupIds{0, 1});
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

    SECTION("Invalid perf section")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    perf:";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::InvalidTasksDef);
    }

    SECTION("Missing perf value")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    perf:\n"
            "      min: 1.0";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::InvalidTasksDef);
    }

    SECTION("Invalid perf type")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    perf:\n"
            "      min: text\n"
            "      max: text";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::InvalidTasksDef);
    }

    SECTION("Invalid perf values")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    perf:\n"
            "      min: 2.0\n"
            "      max: 1.0";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::InvalidTasksDef);
    }

    SECTION("Parse performance")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    perf:\n"
            "      min: 0.8\n"
            "      max: 1.8\n"
            "  agent 2:";
        // clang-format on
        const auto system = angonoka::load_text(text);
        const auto& agent1_perf = system.agents[0].perf;
        REQUIRE(agent1_perf.min == Approx(.8f));
        REQUIRE(agent1_perf.max == Approx(1.8f));
        const auto& agent2_perf = system.agents[1].perf;
        REQUIRE(agent2_perf.min == Approx(.5f));
        REQUIRE(agent2_perf.max == Approx(1.5f));
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
            angonoka::InvalidTasksDef);
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
            angonoka::InvalidTasksDef);
    }

    SECTION("Groups defined in tasks")
    {
        // clang-format off
        constexpr auto text = 
            "tasks:\n"
            "  task1:\n"
            "    group: A\n"
            "    days:\n"
            "      min: 1\n"
            "      max: 3\n"
            "agents:\n"
            "  agent 1:";
        // clang-format on
        const auto system = angonoka::load_text(text);
        REQUIRE(system.groups == angonoka::Groups{"A"});
        REQUIRE(system.agents[0].group_ids == angonoka::GroupIds{0});
    }
}

#undef ANGONOKA_COMMON_YAML
