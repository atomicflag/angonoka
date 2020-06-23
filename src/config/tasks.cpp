#include "../common.h"
#include "../exceptions.h"
#include "load.h"
#include <fmt/format.h>
#include <gsl/gsl-lite.hpp>
#include <range/v3/algorithm/find.hpp>

namespace {
using namespace angonoka;
/**
    Parses task duration.

    Parses blocks such as these:

    duration:
      min: 1 day
      max: 3 days

    @param duration  A map with min/max values or a scalar
    @param task      Task object
*/
void parse_duration(const YAML::Node& duration, Task::Duration& dur)
{
    using detail::parse_duration;
    if (duration.IsScalar()) {
        const auto value = parse_duration(duration.Scalar());
        dur.min = value;
        dur.max = value;
    } else {
        dur.min = parse_duration(duration["min"].Scalar());
        dur.max = parse_duration(duration["max"].Scalar());
    }
    if (dur.min > dur.max) {
        constexpr auto text = "Task's duration minimum can't be "
                              "greater than maximum.";
        throw ValidationError{text};
    }
}

/**
    Parses task group.

    Parses blocks such as these:

    group: A

    @param group_node Scalar holding the name of the group
    @param task       An instance of Task
    @param system     An instance of System
*/
void parse_task_group(
    const YAML::Node& group_node,
    Task& task,
    System& system)
{
    const auto& group_name = group_node.Scalar();
    Expects(!group_name.empty());
    const auto [gid, is_inserted]
        = detail::find_or_insert_group(system.groups, group_name);
    if (is_inserted && !system.has_universal_agents()) {
        constexpr auto text = R"_(No suitable agent for task "{}")_";
        throw ValidationError{fmt::format(text, group_name)};
    }
    task.group_id = gid;
}

/**
    Check for duplicate tasks.

    @param agents An array of Tasks
    @param name   Agent's name
*/
void check_for_duplicates(const Tasks& tasks, std::string_view name)
{
    Expects(!name.empty());
    if (const auto a = ranges::find(tasks, name, &Task::name);
        a != tasks.end()) {
        constexpr auto text = "Duplicate task definition";
        throw ValidationError{text};
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

    @param task_node  Scalar holding the name of the task
    @param task_data  Map with task data
    @param sys        An instance of System
*/
void parse_task(
    const YAML::Node& task_node,
    const YAML::Node& task_data,
    System& sys)
{
    const auto& task_name = task_node.Scalar();
    Expects(!task_name.empty());
    check_for_duplicates(sys.tasks, task_name);
    auto& task = sys.tasks.emplace_back();
    task.name = task_name;
    parse_duration(task_data["duration"], task.duration);

    // Parse task.group
    if (const auto group = task_data["group"]) {
        parse_task_group(group, task, sys);
    }
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
