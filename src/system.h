#pragma once

#include <optional>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

namespace angonoka {
using GroupIds = std::unordered_set<int>;
using Normal = std::normal_distribution<float>;

struct Agent {
	std::string name;
	GroupIds group_ids;
	Normal perf;
};

struct Task {
	std::string name;
	std::optional<int> group_id;
};

using Groups = std::vector<std::string>;
using Agents = std::vector<Agent>;
using Tasks = std::vector<Task>;

struct System {
	Groups groups;
	Agents agents;
	Tasks tasks;
};
} // namespace angonoka
