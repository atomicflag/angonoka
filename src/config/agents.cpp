#include "../exceptions.h"
#include "load.h"
#include <range/v3/algorithm/find.hpp>
#include <string_view>

namespace {
using namespace angonoka;
/**
    Parses agent groups section.

    Parses blocks such as these:

    groups:
      - A
      - B
      - C

    and inserts "A", "B", "C" into Configuration.groups.
    Then places group ids into agent.groups_ids.

    @param group_nodes    Sequence with group names
    @param agent          An instance of Agent
    @param groups         An array of Groups
*/
void parse_agent_groups(
    const YAML::Node& group_nodes,
    Agent& agent,
    Groups& groups)
{
    for (auto&& g : group_nodes) {
        const auto gid
            = detail::find_or_insert_group(groups, g.Scalar());
        agent.group_ids.emplace(gid.first);
    }
}

/**
    Makes sure the performance is greater than 0.

    @param performance  Performance value
    @param agent        Agent

    @return Performance value
*/
float validate_performance(float performance)
{
    if (performance <= 0.F)
        throw std::domain_error{"Negative performance"};
    return performance;
}

/**
    Parses agent performance.

    Parses blocks such as these:

    performance:
      min: 1.0
      max: 2.0

    @param performance  Map with performance data
    @param agent        An instance of Agent
*/
void parse_agent_performance(
    const YAML::Node& performance,
    Agent& agent)
{
    try {
        if (performance.IsScalar()) {
            const auto performance_value
                = validate_performance(performance.as<float>());
            agent.performance.min = performance_value;
            agent.performance.max = performance_value;
        } else {
            agent.performance.min = validate_performance(
                performance["min"].as<float>());
            agent.performance.max = validate_performance(
                performance["max"].as<float>());
        }
    } catch (const std::domain_error&) {
        throw NegativePerformance{agent.name};
    } catch (const YAML::Exception&) {
        throw InvalidAgentPerformance{};
    }
    if (agent.performance.min > agent.performance.max)
        throw AgentPerformanceMinMax{};
}

/**
    Check for duplicate agents.

    @param agents An array of Agents
    @param name   Agent's name
*/
void check_for_duplicates(const Agents& agents, std::string_view name)
{
    Expects(!name.empty());

    if (const auto a = ranges::find(agents, name, &Agent::name);
        a != agents.end())
        throw DuplicateAgentDefinition{};
}

/**
    Parses agent blocks.

    Parses blocks such as these:

    agent 1:
      performance:
        min: 0.5
        max: 1.5
      groups:
        - A
        - B

    @param agent_node Scalar holding the name of the agent
    @param agent_data   Map with agent data
    @param config       An instance of Configuration
*/
void parse_agent(
    const YAML::Node& agent_node,
    const YAML::Node& agent_data,
    Configuration& config)
{
    const auto& agent_name = agent_node.Scalar();
    Expects(!agent_name.empty());

    check_for_duplicates(config.agents, agent_name);
    auto& agent = config.agents.emplace_back();

    // Parse agent.name
    agent.name = agent_name;

    // Parse agent.groups
    if (const auto groups = agent_data["groups"]) {
        parse_agent_groups(groups, agent, config.groups);
    }

    // Parse agent.perf
    if (const auto performance = agent_data["performance"]) {
        parse_agent_performance(performance, agent);
    }
}
} // namespace

namespace angonoka::detail {
void parse_agents(const YAML::Node& node, Configuration& config)
{
    for (auto&& agent : node) {
        parse_agent(agent.first, agent.second, config);
    }
}
} // namespace angonoka::detail
