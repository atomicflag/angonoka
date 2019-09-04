#pragma once

#include <string>
#include <unordered_set>
#include <random>

namespace angonoka {
	using GroupIds = std::unordered_set<int>;
	using Normal = std::normal_distribution<float>;

	struct Agent {
		std::string name;
		GroupIds group_ids;
		Normal perf;
	};
}
