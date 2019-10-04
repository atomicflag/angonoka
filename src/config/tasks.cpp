#include "tasks.h"

#include "../common.h"

namespace angonoka::detail {
/**
	Parses task blocks.

	Parses blocks such as these:

	task 1:
		group: A

	@param task_node	Scalar holding the name of the task
	@param task_data	Map with task data
	@param sys			An instance of System
*/
void parse_task(const YAML::Node& /* task_node */,
	const YAML::Node& /* task_data */, System& /* sys */)
{
	// TODO: WIP
}

void parse_tasks(const YAML::Node& node, System& sys)
{
	for (auto&& task : node) {
		parse_task(task.first, task.second, sys);
	}
}
} // namespace angonoka::detail
