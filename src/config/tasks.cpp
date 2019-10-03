#include "tasks.h"

#include "../common.h"
#include "../system.h"
#include "errors.h"

#include <fmt/format.h>
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

void validate_task(
	const YAML::Node& task_node, const YAML::Node& task_data)
{
	if (task_data.IsSequence() || task_data.IsScalar()
		|| !task_data.IsDefined()) {
		constexpr auto err_text
			= "Invalid task specification for \"{}\"";
		throw InvalidTasksDefError {fmt::format(err_text, task)};
	}
}

/**
	Parses task blocks.

	Parses blocks such as these:

	task 1:
		group: A

	@param task_node	Scalar holding the name of the task
	@param task_data	Map with task data
	@param sys			An instance of System
*/
void parse_task(const YAML::Node& task_node,
	const YAML::Node& task_data, System& sys)
{
	// TODO: WIP
}

void parse_tasks(const YAML::Node& node, System& sys)
{
	for (auto&& task : node) {
		validate_task(task.first, task.second);
		parse_task(task.first, task.second, sys);
	}
}
} // namespace angonoka::detail
