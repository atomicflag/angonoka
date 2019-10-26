#pragma once

#include "../system.h"
#include <yaml-cpp/yaml.h>

namespace angonoka {
/**
	Load System from a YAML string.

	@param text Null-terminated string

	@return An instance of System
*/
System load_text(const char* text);
} // namespace angonoka

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

/**
	Parses tasks blocks.

	Parses blocks such as these:

	tasks:
		task 1:
			days:
				min: 1
				max: 3
		task 2:
			days:
				min: 2
				max: 2

	@param node "tasks" node
	@param sys An instance of System
*/
void parse_tasks(const YAML::Node& node, System& sys);
} // namespace angonoka::detail
