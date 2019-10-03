#include "agents.h"

#include "../common.h"
#include "../system.h"
#include "errors.h"

#include <fmt/format.h>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/iota.hpp>
#include <string_view>
#include <yaml-cpp/yaml.h>

namespace angonoka::detail {
/**
	Create mean and stddev from min and max values.

	Mean is an average of min and max.

	mean = (min + max) / 2

	Stddev is the difference between the mean and min/max
	divided by stdnum.

	stddev = (max - mean) / stdnum

	@param min 		Lower bound
	@param max 		Upper bound
	@param stdnum 	Size of stddev (1 by default)

	@return A tuple with mean and stddev
*/
std::tuple<float, float> make_normal_params(
	float min, float max, float stdnum = 1.F)
{
	const float mean = (min + max) / 2.F;
	return {mean, (max - mean) / stdnum};
}

/**
	Finds or inserts a group into System.groups.

	@param sys		System instance
	@param group	Group name

	@return Index of the group in System.groups
*/
Int find_or_insert_group(System& sys, std::string_view group)
{
	if (const auto& f = ranges::find(sys.groups, group);
		f != sys.groups.end())
		return std::distance(sys.groups.begin(), f);
	sys.groups.emplace_back(group);
	return sys.groups.size() - 1;
}

void validate_agents(const YAML::Node& node)
{
	if (!node) {
		constexpr auto err_text = "Missing \"agents\" section";
		throw InvalidTasksDefError {err_text};
	}
	if (!node.IsMap()) {
		constexpr auto err_text = "Section \"agents\" has an invalid "
								  "type";
		throw InvalidTasksDefError {err_text};
	}
}

void validate_agent_groups(const YAML::Node& groups, Agent& agent)
{
	if (!groups.IsSequence()) {
		constexpr auto err_text
			= "Invalid groups specification for \"{}\"";
		throw InvalidTasksDefError {
			fmt::format(err_text, agent.name)};
	}
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
	using detail::find_or_insert_group;
	for (auto&& g : groups) {
		const auto gid = find_or_insert_group(sys, g.Scalar());
		agent.group_ids.emplace(gid);
	}
}

void validate_agent(
	const YAML::Node& agent, const YAML::Node& agent_data)
{
	if (agent_data.IsSequence() || agent_data.IsScalar()
		|| !agent_data.IsDefined()) {
		constexpr auto err_text
			= "Invalid agent specification for \"{}\"";
		throw InvalidTasksDefError {fmt::format(err_text, agent)};
	}
}

void validate_agent_perf(const YAML::Node& perf, Agent& agent)
{
	if (!perf.IsMap() || !perf["min"] || !perf["max"]) {
		constexpr auto err_text
			= "Invalid perf specification for \"{}\"";
		throw InvalidTasksDefError {
			fmt::format(err_text, agent.name)};
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
	const auto [mean, stddev] = make_normal_params(
		perf["min"].as<float>(.5F), perf["max"].as<float>(1.5F), 3.F);
	agent.perf = Normal {mean, stddev};
}

/**
	Assigns a default performance values.

	Currently the default is Normal(1, 1.5)

	@param agent An instance of Agent
*/
void assign_default_perf(Agent& agent)
{
	constexpr auto stddev = 0.5F / 3.F;
	constexpr auto mean = 1.F;
	agent.perf = Normal {mean, stddev};
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
		validate_agent_groups(groups, agent);
		parse_agent_groups(groups, agent, sys);
	}

	// Parse agent.perf
	if (const auto perf = agent_data["perf"]) {
		validate_agent_perf(perf, agent);
		parse_agent_perf(perf, agent);
	} else {
		assign_default_perf(agent);
	}
}

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
		validate_agent(agent.first, agent.second);
		parse_agent(agent.first, agent.second, sys);
	}
}
} // namespace angonoka::detail
