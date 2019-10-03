#include <catch2/catch.hpp>

#include "config/errors.h"
#include "config/load.h"

#define ANGONOKA_COMMON_YAML                                         \
	"tasks:\n"                                                       \
	"  task1:\n"

TEST_CASE("Loading agents")
{
	SECTION("No 'agents' section")
	{
		constexpr auto text = ANGONOKA_COMMON_YAML;
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Section 'agents' has an invalid type")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents: 123";
		// clang-format on
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Invalid agent spec")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1: 123";
		// clang-format on
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Invalid group spec")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1:\n"
			"    groups: 123";
		// clang-format on
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Parse groups")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1:\n"
			"    groups:\n"
			"      - A\n"
			"      - B\n"
			"  agent 2:\n"
			"    groups:\n"
			"      - A\n"
			"      - C\n";
		// clang-format on
		const auto system = angonoka::load_text(text);
		REQUIRE(system.groups == angonoka::Groups {"A", "B", "C"});
		REQUIRE(system.agents.size() == 2);
		// Agent 1 has A(0) and B(1)
		REQUIRE(
			system.agents[0].group_ids == angonoka::GroupIds {0, 1});
		// Agent 2 has A(0) and C(2)
		REQUIRE(
			system.agents[1].group_ids == angonoka::GroupIds {0, 2});
	}

	SECTION("Fill empty groups")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1:\n"
			"    groups:\n"
			"      - A\n"
			"      - B\n"
			"  agent 2:";
		// clang-format on
		const auto system = angonoka::load_text(text);
		// Agent 1 has A(0) and B(1)
		REQUIRE(
			system.agents[0].group_ids == angonoka::GroupIds {0, 1});
		// Agent 2 should have all groups
		REQUIRE(
			system.agents[1].group_ids == angonoka::GroupIds {0, 1});
	}

	SECTION("No groups")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1:\n"
			"  agent 2:";
		// clang-format on
		const auto system = angonoka::load_text(text);
		REQUIRE(system.agents[0].group_ids.empty());
		REQUIRE(system.agents[1].group_ids.empty());
	}

	SECTION("Invalid perf section")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1:\n"
			"    perf:\n";
		// clang-format on
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Missing perf value")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1:\n"
			"    perf:\n"
			"      min: 1.0\n";
		// clang-format on
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Invalid perf type")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1:\n"
			"    perf:\n"
			"      min: text\n"
			"      max: text\n";
		// clang-format on
		const auto system = angonoka::load_text(text);
		REQUIRE(system.agents[0].perf.mean() == Approx(1.f));
		REQUIRE(system.agents[0].perf.stddev() == Approx(0.5f / 3.f));
	}

	SECTION("Parse performance")
	{
		// clang-format off
		constexpr auto text = 
			ANGONOKA_COMMON_YAML
			"agents:\n"
			"  agent 1:\n"
			"    perf:\n"
			"      min: 0.5\n"
			"      max: 1.5\n"
			"  agent 2:\n";
		// clang-format on
		const auto system = angonoka::load_text(text);
		REQUIRE(system.agents[0].perf.mean() == Approx(1.f));
		REQUIRE(system.agents[0].perf.stddev() == Approx(0.5f / 3.f));
		REQUIRE(system.agents[1].perf.mean() == Approx(1.f));
		REQUIRE(system.agents[1].perf.stddev() == Approx(0.5f / 3.f));
	}
}

#undef ANGONOKA_COMMON_YAML
