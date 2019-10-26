#include "../common.h"
#include "../exceptions.h"
#include "load.h"
#include <range/v3/algorithm/find.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/iota.hpp>
#include <string_view>

namespace {
using namespace angonoka;
/**
	Finds or inserts a group into System.groups.

	@param sys		System instance
	@param group	Group name

	@return Index of the group in System.groups
*/
Int find_or_insert_group(System& sys, std::string_view group)
{
	if (const auto f = ranges::find(sys.groups, group);
		f != sys.groups.end())
		return std::distance(sys.groups.begin(), f);
	sys.groups.emplace_back(group);
	return sys.groups.size() - 1;
}

/**
	Parses agent groups section.

	Parses blocks such as these:

	groups:
		- A
		- B
		- C

	and inserts "A", "B", "C" into System.groups.
	Then places group ids into agent.groups_ids.

	@param groups	Sequence with group names
	@param agent	An instance of Agent
	@param sys		An instance of System
*/
void parse_agent_groups(
	const YAML::Node& groups, Agent& agent, System& sys)
{
	for (auto&& g : groups) {
		const auto gid = find_or_insert_group(sys, g.Scalar());
		agent.group_ids.emplace(gid);
	}
}

/**
	Parses agent perf.

	Parses blocks such as these:

	perf:
		min: 1.0
		max: 2.0

	@param perf		Map with perf data
	@param agent	An instance of Agent
*/
void parse_agent_perf(const YAML::Node& perf, Agent& agent)
{
	try {
		agent.perf.min = perf["min"].as<float>();
		agent.perf.max = perf["max"].as<float>();
	} catch (const YAML::Exception&) {
		throw InvalidTasksDef{"Invalid agent performance."};
	}
	if (agent.perf.min > agent.perf.max) {
		constexpr auto text = "Agent's performance minimum can't be "
							  "greater than maximum.";
		throw InvalidTasksDef{text};
	}
}

/**
	Parses agent blocks.

	Parses blocks such as these:

	agent 1:
		perf:
			min: 0.5
			max: 1.5
		groups:
			- A
			- B

	@param agent_node	Scalar holding the name of the agent
	@param agent_data	Map with agent data
	@param sys			An instance of System
*/
void parse_agent(const YAML::Node& agent_node,
	const YAML::Node& agent_data, System& sys)
{
	auto& agent = sys.agents.emplace_back();

	// Parse agent.name
	agent.name = agent_node.Scalar();

	// Parse agent.groups
	if (const auto groups = agent_data["groups"]) {
		parse_agent_groups(groups, agent, sys);
	}

	// Parse agent.perf
	if (const auto perf = agent_data["perf"]) {
		parse_agent_perf(perf, agent);
	}
}
} // namespace

namespace angonoka::detail {
void fill_empty_groups(System& sys)
{
	using ranges::to;
	using namespace ranges::views;
	for (auto&& a : sys.agents) {
		if (!a.group_ids.empty()) continue;
		a.group_ids = to<GroupIds>(
			iota(0, static_cast<int>(sys.groups.size())));
	}
}

void parse_agents(const YAML::Node& node, System& sys)
{
	for (auto&& agent : node) {
		parse_agent(agent.first, agent.second, sys);
	}
}
} // namespace angonoka::detail
