#include <yaml-cpp/yaml.h>

#include "agents.h"
#include "load.h"
#include "tasks.h"

namespace angonoka {
/**
	Loads agents and groups definitions.

	@param agents	A YAML node containing agents
	@param system	An instance of System
*/
void load_agents(const YAML::Node& agents, System& system)
{
	detail::validate_agents(agents);
	detail::parse_agents(agents, system);
	if (!system.groups.empty()) detail::fill_empty_groups(system);
}

/**
	Loads tasks definitions.

	@param tasks	A YAML node containing tasks
	@param system	An instance of System
*/
void load_tasks(const YAML::Node& tasks, System& /* system */)
{
	detail::validate_tasks(tasks);
	// TODO: WIP
}

System load_text(const char* text)
{
	const auto node = YAML::Load(text);
	System system;
	load_agents(node["agents"], system);
	load_tasks(node["tasks"], system);
	return system;
}
} // namespace angonoka
