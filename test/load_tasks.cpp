#include <catch2/catch.hpp>

#include "config/errors.h"
#include "config/load.h"

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
}
