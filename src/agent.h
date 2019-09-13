#pragma once

#include <random>
#include <string>
#include <unordered_set>

namespace angonoka {
using GroupIds = std::unordered_set<int>;
using Normal = std::normal_distribution<float>;

struct Agent {
	std::string name;
	GroupIds group_ids;
	Normal perf;
};
} // namespace angonoka
