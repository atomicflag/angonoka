#include "config/load.h"
#include "exceptions.h"
#include <boost/ut.hpp>

using namespace boost::ut;

#define ANGONOKA_COMMON_YAML                                         \
    "tasks:\n"                                                       \
    "  - name: task1\n"                                              \
    "    duration:\n"                                                \
    "      min: 1 day\n"                                             \
    "      max: 3 days\n"

suite loading_agents = [] {
    using namespace std::literals::string_view_literals;

    "no 'agents' section"_test = [] {
        constexpr auto text = ANGONOKA_COMMON_YAML;
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "empty section"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents: {}";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "section 'agents' has an invalid type"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents: 123";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "invalid agent spec"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1: 123";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "invalid group spec"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    groups: 123";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "extra attributes"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    asdf: 123";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "parse groups"_test = [] {
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
        expect(config.groups == angonoka::Groups{"A", "B", "C"});
        expect(config.agents.size() == 2_i);
        // Agent 1 has A(0) and B(1)
        expect(
            config.agents[0].group_ids
            == angonoka::GroupIndices{0, 1});
        // Agent 2 has A(0) and C(2)
        expect(
            config.agents[1].group_ids
            == angonoka::GroupIndices{0, 2});
    };

    "universal agents"_test = [] {
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
        expect(
            config.agents[0].group_ids
            == angonoka::GroupIndices{0, 1});
        // Agent 2 should be universal
        expect(is_universal(config.agents[1]));
        expect(can_work_on(config.agents[1], 0));
        expect(can_work_on(config.agents[1], 1));
    };

    "no groups"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "  agent 2:";
        // clang-format on
        const auto config = angonoka::load_text(text);
        expect(config.agents[0].group_ids.empty());
        expect(config.agents[1].group_ids.empty());
    };

    "invalid performance section"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "missing performance value"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:\n"
            "      min: 1.0";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "invalid performance type, text"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:\n"
            "      min: text\n"
            "      max: text";
        // clang-format on
        expect(throws<angonoka::InvalidAgentPerformance>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::InvalidAgentPerformance& e) {
                expect(eq(
                    e.what(),
                    R"(Agent "agent 1" has invalid performance.)"sv));
                throw;
            }
        }));
    };

    "invalid performance type, dict"_test = [] {
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
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "invalid performance values"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance:\n"
            "      min: 2.0\n"
            "      max: 1.0";
        // clang-format on
        expect(throws<angonoka::AgentPerformanceMinMax>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::AgentPerformanceMinMax& e) {
                expect(eq(
                    e.what(),
                    R"(The minimum performance of the agent "agent 1" is greater than maximum.)"sv));
                throw;
            }
        }));
    };

    "parse performance"_test = [] {
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
        expect(agent1_performance.min == .8_d);
        expect(agent1_performance.max == 1.8_d);
        const auto& agent2_performance = config.agents[1].performance;
        expect(agent2_performance.min == 1._d);
        expect(agent2_performance.max == 1._d);
    };

    "duplicate agents"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "  agent 1:";
        // clang-format on
        expect(throws<angonoka::DuplicateAgentDefinition>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::DuplicateAgentDefinition& e) {
                expect(eq(
                    e.what(),
                    R"(Agent "agent 1" is specified more than once.)"sv));
                throw;
            }
        }));
    };

    "duplicate agent sections"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "agents:\n"
            "  agent 2:";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "exact performance"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance: 1.0\n";
        // clang-format on
        const auto config = angonoka::load_text(text);
        const auto& agent_performance = config.agents[0].performance;
        expect(agent_performance.min == 1._d);
        expect(agent_performance.max == 1._d);
    };

    "agent performance validation"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "agents:\n"
            "  agent 1:\n"
            "    performance: -1.0\n";
        // clang-format on
        expect(throws<angonoka::NegativePerformance>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::NegativePerformance& e) {
                expect(eq(
                    e.what(),
                    R"(Agent "agent 1" can't have a negative performance value.)"sv));
                throw;
            }
        }));
    };
};

#undef ANGONOKA_COMMON_YAML
