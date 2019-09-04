#include <yaml-cpp/yaml.h>

#include "load.h"
#include "agents.h"

namespace angonoka {
	System load_text(const char* text) {
		const auto node = YAML::Load(text);
		System system;
		const auto agents = node["agents"];
		detail::validate_agents(agents);
		detail::parse_agents(agents, system);
		if(!system.groups.empty())
			detail::fill_empty_groups(system);
		return system;
	}
} // namespace angonoka
