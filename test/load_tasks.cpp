#include "config/errors.h"
#include "config/load.h"
#include <catch2/catch.hpp>

#define ANGONOKA_COMMON_YAML                                         \
	"agents:\n"                                                      \
	"  agent1:\n"

TEST_CASE("Loading tasks")
{
	SECTION("No 'tasks' section")
	{
		constexpr auto text = ANGONOKA_COMMON_YAML;
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Empty 'tasks' section")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"tasks:";
		// clang-format on
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Invalid 'tasks' format")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"tasks: 123";
		// clang-format on
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	// TODO: WIP

	// SECTION("A single task")
	// {
	// 	// clang-format off
	// 	constexpr auto text =
	// 		ANGONOKA_COMMON_YAML
	// 		"tasks:\n"
	// 		"  task 1:\n"
	// 		"    days:\n"
	// 		"      min: 1\n"
	// 		"      max: 3";
	// 	// clang-format on
	// 	const auto system = angonoka::load_text(text);
	// 	REQUIRE(system.tasks.size() == 1);
	// }
}
