#include "config/load.h"
#include "exceptions.h"
#include <catch2/catch.hpp>

#define ANGONOKA_COMMON_YAML                                         \
	"agents:\n"                                                      \
	"  agent1:\n"

TEST_CASE("Loading tasks")
{
	SECTION("No 'tasks' section")
	{
		constexpr auto text = ANGONOKA_COMMON_YAML;
		REQUIRE_THROWS_AS(
			angonoka::load_text(text), angonoka::InvalidTasksDef);
	}

	SECTION("Empty 'tasks' section")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"tasks:";
		// clang-format on
		REQUIRE_THROWS_AS(
			angonoka::load_text(text), angonoka::InvalidTasksDef);
	}

	SECTION("Invalid 'tasks' format")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"tasks: 123";
		// clang-format on
		REQUIRE_THROWS_AS(
			angonoka::load_text(text), angonoka::InvalidTasksDef);
	}

	SECTION("A single task")
	{
		// clang-format off
		constexpr auto text =
			ANGONOKA_COMMON_YAML
			"tasks:\n"
			"  task 1:\n"
			"    days:\n"
			"      min: 1\n"
			"      max: 3";
		// clang-format on
		const auto system = angonoka::load_text(text);
		REQUIRE(system.tasks.size() == 1);
		const auto& task = system.tasks[0];
		REQUIRE(task.name == "task 1");
		REQUIRE_FALSE(task.group_id);
		constexpr int secs_in_day = 60 * 60 * 24;
		REQUIRE(task.dur.min == secs_in_day);
		REQUIRE(task.dur.max == secs_in_day * 3);
	}

	SECTION("Invalid task duration type")
	{
		// clang-format off
		constexpr auto text =
			ANGONOKA_COMMON_YAML
			"tasks:\n"
			"  task 1:\n"
			"    days:\n"
			"      min: as\n"
			"      max: a";
		// clang-format on
		REQUIRE_THROWS_AS(
			angonoka::load_text(text), angonoka::InvalidTasksDef);
	}

	SECTION("Invalid task duration")
	{
		// clang-format off
		constexpr auto text =
			ANGONOKA_COMMON_YAML
			"tasks:\n"
			"  task 1:\n"
			"    days:\n"
			"      min: 5\n"
			"      max: 2";
		// clang-format on
		REQUIRE_THROWS_AS(
			angonoka::load_text(text), angonoka::InvalidTasksDef);
	}

	SECTION("Valid group id")
	{
		// clang-format off
		constexpr auto text =
			"agents:\n"
			"  agent1:\n"
			"    groups:\n"
			"      - A\n"
			"tasks:\n"
			"  task 1:\n"
			"    group: A\n"
			"    days:\n"
			"      min: 1\n"
			"      max: 2";
		// clang-format on
		const auto system = angonoka::load_text(text);
		REQUIRE(system.tasks.size() == 1);
		const auto& task = system.tasks[0];
		// group_id is std::optional and has to have a value
		REQUIRE(task.group_id);
		REQUIRE(task.group_id == 0);
	}

	SECTION("Two tasks, one group")
	{
		// clang-format off
		constexpr auto text =
			"agents:\n"
			"  agent1:\n"
			"    groups:\n"
			"      - A\n"
			"tasks:\n"
			"  task 1:\n"
			"    group: A\n"
			"    days:\n"
			"      min: 1\n"
			"      max: 2\n"
			"  task 2:\n"
			"    group: A\n"
			"    days:\n"
			"      min: 1\n"
			"      max: 2";
		// clang-format on
		const auto system = angonoka::load_text(text);
		REQUIRE(system.tasks.size() == 2);
		const auto& task1 = system.tasks[0];
		const auto& task2 = system.tasks[1];
		REQUIRE(task1.group_id == 0);
		REQUIRE(task2.group_id == 0);
	}

	SECTION("Duplicate tasks")
	{
		// clang-format off
		constexpr auto text =
			ANGONOKA_COMMON_YAML
			"tasks:\n"
			"  task 1:\n"
			"    days:\n"
			"      min: 1\n"
			"      max: 2\n"
			"  task 1:\n"
			"    days:\n"
			"      min: 1\n"
			"      max: 2";
		// clang-format on
		REQUIRE_THROWS_AS(
			angonoka::load_text(text), angonoka::InvalidTasksDef);
	}

	// TODO: WIP
	// Check group_id
	// 	- Invalid group
	// 		If a task has a group that wasn't defined in any of the
	// 		agent definitions, it should produce an error
}
