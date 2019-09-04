#pragma once

// Forward declaration
namespace angonoka {
	struct System;
} // namespace angonoka

// Forward declaration
namespace YAML {
	class Node;
} // namespace YAML

namespace angonoka::detail {
	/**
		Fills agent.group_ids with all available group ids
		when it wasn't specified in YAML.

		E.g.

		agents:
			agent 1:
				groups:
					- A
					- B
			agent 2:

		"agent 2" will implicitly have groups "A" and "B"

		@param sys An instance of System
	*/
	void fill_empty_groups(System& sys);

	void validate_agents(const YAML::Node& node);

	/**
		Parses agents blocks.

		Parses blocks such as these:

		agents:
			agent 1:
			agent 2:

		@param node "agents" node
		@param sys An instance of System
	*/
	void parse_agents(const YAML::Node& node, System& sys);
} // namespace angonoka::detail
