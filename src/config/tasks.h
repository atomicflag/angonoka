#pragma once

#include "../system.h"
#include <yaml-cpp/yaml.h>

namespace angonoka::detail {
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
