#include "load.h"
#include "agents.h"
#include "tasks.h"
#include "validation.h"
#include <yaml-cpp/yaml.h>

namespace angonoka {
void validate_configuration(const YAML::Node& node)
{
	using namespace validation;
	// clang-format off
	constexpr auto schema = attributes(
		required("agents",
			map(attributes(
				optional("perf", attributes(
					required("min", scalar()),
					required("max", scalar())
				)),
				optional("groups", sequence(scalar()))
			))
		),
		required("tasks",
			map(attributes(
				optional("group", scalar())
			))
		)
	);
	// clang-format on
	schema(node);
}

System load_text(const char* text)
{
	const auto node = YAML::Load(text);
	validate_configuration(node);
	System system;
	detail::parse_agents(node["agents"], system);
	if (!system.groups.empty()) detail::fill_empty_groups(system);
	detail::parse_tasks(node["tasks"], system);
	return system;
}
} // namespace angonoka
