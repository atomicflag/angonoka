#include "config/load.h"
#include "exceptions.h"
#include <catch2/catch.hpp>

#define ANGONOKA_COMMON_YAML                                         \
    "tasks:\n"                                                       \
    "  - name: task1\n"                                              \
    "    duration:\n"                                                \
    "      min: 1 day\n"                                             \
    "      max: 3 days\n"

TEST_CASE("loading agents")
{
    using namespace std::literals::string_view_literals;
    using Catch::Message;

    SECTION("no 'agents' section")
    {
        constexpr auto text = ANGONOKA_COMMON_YAML;

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("empty section")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents: {}";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("section 'agents' has an invalid type")
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

    SECTION("invalid agent spec")
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

    SECTION("invalid group spec")
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

    SECTION("extra attributes")
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

    SECTION("parse groups")
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

        const auto config = angonoka::load_text(text);

        REQUIRE(config.groups == angonoka::Groups{"A", "B", "C"});
        REQUIRE(config.agents.size() == 2);
        // Agent 1 has A(0) and B(1)
        REQUIRE(
            config.agents[0].group_ids
            == angonoka::GroupIndices{0, 1});
        // Agent 2 has A(0) and C(2)
        REQUIRE(
            config.agents[1].group_ids
            == angonoka::GroupIndices{0, 2});
    }

    SECTION("universal agents")
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

        const auto config = angonoka::load_text(text);

        // Agent 1 has A(0) and B(1)
        REQUIRE(
            config.agents[0].group_ids
            == angonoka::GroupIndices{0, 1});
        // Agent 2 should be universal
        REQUIRE(is_universal(config.agents[1]));
        REQUIRE(can_work_on(config.agents[1], 0));
        REQUIRE(can_work_on(config.agents[1], 1));
    }

    SECTION("no groups")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "  agent 2:";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.agents[0].group_ids.empty());
        REQUIRE(config.agents[1].group_ids.empty());
    }

    SECTION("agent ids")
    {
        using angonoka::AgentIndex;

        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "  agent 2:";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.agents[0].id == AgentIndex{0});
        REQUIRE(config.agents[1].id == AgentIndex{1});
    }

    SECTION("invalid performance section")
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

    SECTION("missing performance value")
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

    SECTION("invalid performance type, text")
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

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::InvalidAgentPerformance,
            Message(R"(Agent "agent 1" has invalid performance.)"));
    }

    SECTION("invalid performance type, dict")
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

    SECTION("invalid performance values")
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

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::AgentPerformanceMinMax,
            Message(
                R"(The minimum performance of the agent "agent 1" is greater than maximum.)"));
    }

    SECTION("parse performance")
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

        const auto config = angonoka::load_text(text);

        const auto& agent1_performance = config.agents[0].performance;
        REQUIRE(agent1_performance.min == Approx(.8));
        REQUIRE(agent1_performance.max == Approx(1.8));
        const auto& agent2_performance = config.agents[1].performance;
        REQUIRE(agent2_performance.min == Approx(1.));
        REQUIRE(agent2_performance.max == Approx(1.));
    }

    SECTION("duplicate agents")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "  agent 1:";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::DuplicateAgentDefinition,
            Message(
                R"(Agent "agent 1" is specified more than once.)"));
    }

    SECTION("duplicate agent sections")
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

    SECTION("exact performance")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance: 1.0\n";
        // clang-format on

        const auto config = angonoka::load_text(text);

        const auto& agent_performance = config.agents[0].performance;
        REQUIRE(agent_performance.min == Approx(1.));
        REQUIRE(agent_performance.max == Approx(1.));
    }

    SECTION("agent performance validation")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance: -1.0\n";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::NegativePerformance,
            Message(
                R"(Agent "agent 1" can't have a negative performance value.)"));
    }
}

#undef ANGONOKA_COMMON_YAML
