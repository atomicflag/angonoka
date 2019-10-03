#pragma once

#include <string>
#include <vector>
#include <random>
#include <unordered_set>

namespace angonoka {
using GroupIds = std::unordered_set<int>;
using Normal = std::normal_distribution<float>;

struct Agent {
	std::string name;
	GroupIds group_ids;
	Normal perf;
};

using Groups = std::vector<std::string>;
using Agents = std::vector<Agent>;

struct System {
	Groups groups;
	Agents agents;
};
} // namespace angonoka
