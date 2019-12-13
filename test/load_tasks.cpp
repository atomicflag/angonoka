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

	// TODO: WIP
	// Check group_id
	// 	- Invalid group
	// 	- Type
	// Check duplicate task definitions

	// SECTION("Valid group id")
	// {
	// 	// clang-format off
	// 	constexpr auto text =
	// 		ANGONOKA_COMMON_YAML
	// 		"tasks:\n"
	// 		"  task 1:\n"
	// 		"    group: A\n"
	// 		"    days:\n"
	// 		"      min: 1\n"
	// 		"      max: 2";
	// 	// clang-format on
	// 	const auto system = angonoka::load_text(text);
	// 	REQUIRE(system.tasks.size() == 1);
	// 	const auto& task = system.tasks[0];
	// 	REQUIRE(task.group_id);
	// }
}
