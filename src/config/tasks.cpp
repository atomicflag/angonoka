#include "tasks.h"

#include "../common.h"
#include "../system.h"
#include "errors.h"

#include <yaml-cpp/yaml.h>

namespace angonoka::detail {
void validate_tasks(const YAML::Node& node)
{
	if (!node) {
		constexpr auto err_text = "Missing \"tasks\" section";
		throw InvalidTasksDefError {err_text};
	}
	if (!node.IsMap()) {
		constexpr auto err_text = "Section \"tasks\" has an invalid "
								  "type";
		throw InvalidTasksDefError {err_text};
	}
}

void parse_tasks(const YAML::Node& /* node */, System& /* sys */)
{
	// TODO: WIP
}
} // namespace angonoka::detail
