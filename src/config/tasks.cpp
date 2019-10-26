#include "../common.h"
#include "../exceptions.h"
#include "load.h"

namespace {
using namespace angonoka;
/**
	Parses task duration.

	Parses blocks such as these:

	days:
		min: 1
		min: 3

	@param days	Map with task duration
	@param task	Task object
*/
void parse_days(const YAML::Node& days, Task& task)
{
	constexpr int secs_in_day = 60 * 60 * 24;
	try {
		task.dur.min = days["min"].as<int>() * secs_in_day;
		task.dur.max = days["max"].as<int>() * secs_in_day;
	} catch (const YAML::Exception&) {
		throw InvalidTasksDef{"Invalid task duration."};
	}
	if (task.dur.min > task.dur.max) {
		constexpr auto text = "Task's duration minimum can't be "
							  "greater than maximum.";
		throw InvalidTasksDef{text};
	}
}

/**
	Parses task blocks.

	Parses blocks such as these:

	task 1:
		group: A
		days:
			min: 2
			max: 2

	@param task_node	Scalar holding the name of the task
	@param task_data	Map with task data
	@param sys			An instance of System
*/
void parse_task(const YAML::Node& task_node,
	const YAML::Node& task_data, System& sys)
{
	auto& task = sys.tasks.emplace_back();
	task.name = task_node.Scalar();
	parse_days(task_data["days"], task);
}
} // namespace

namespace angonoka::detail {
void parse_tasks(const YAML::Node& node, System& sys)
{
	for (auto&& task : node) {
		parse_task(task.first, task.second, sys);
	}
}
} // namespace angonoka::detail
