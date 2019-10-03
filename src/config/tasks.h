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
void validate_tasks(const YAML::Node& node);

/**
	Parses tasks blocks.

	Parses blocks such as these:

	tasks:
		task 1:
		task 2:

	@param node "tasks" node
	@param sys An instance of System
*/
void parse_tasks(const YAML::Node& node, System& sys);
} // namespace angonoka::detail
