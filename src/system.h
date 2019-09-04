#pragma once

#include <vector>
#include <string>

#include "agent.h"

namespace angonoka {
	using Groups = std::vector<std::string>;
	using Agents = std::vector<Agent>;

	struct System {
		Groups groups;
		Agents agents;
	};
}
