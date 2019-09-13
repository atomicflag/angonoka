#include <catch2/catch.hpp>

#include "config/errors.h"
#include "config/load.h"

TEST_CASE("Loading yaml")
{
	SECTION("No 'agents' section")
	{
		REQUIRE_THROWS_AS(
			angonoka::load_text(""), angonoka::InvalidTasksDefError);
	}

	SECTION("Section 'agents' has an invalid type")
	{
		constexpr auto text = "agents: 123";
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Invalid agent spec")
	{
		constexpr auto text = "agents:\n"
							  "  agent 1: 123";
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Invalid group spec")
	{
		constexpr auto text = "agents:\n"
							  "  agent 1:\n"
							  "    groups: 123";
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Parse groups")
	{
		constexpr auto text = "agents:\n"
							  "  agent 1:\n"
							  "    groups:\n"
							  "      - A\n"
							  "      - B\n"
							  "  agent 2:\n"
							  "    groups:\n"
							  "      - A\n"
							  "      - C\n";
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
		constexpr auto text = "agents:\n"
							  "  agent 1:\n"
							  "    groups:\n"
							  "      - A\n"
							  "      - B\n"
							  "  agent 2:";
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
		constexpr auto text = "agents:\n"
							  "  agent 1:\n"
							  "  agent 2:";
		const auto system = angonoka::load_text(text);
		REQUIRE(system.agents[0].group_ids.empty());
		REQUIRE(system.agents[1].group_ids.empty());
	}

	SECTION("Invalid perf section")
	{
		constexpr auto text = "agents:\n"
							  "  agent 1:\n"
							  "    perf:\n";
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Missing perf value")
	{
		constexpr auto text = "agents:\n"
							  "  agent 1:\n"
							  "    perf:\n"
							  "      min: 1.0\n";
		REQUIRE_THROWS_AS(angonoka::load_text(text),
			angonoka::InvalidTasksDefError);
	}

	SECTION("Invalid perf type")
	{
		constexpr auto text = "agents:\n"
							  "  agent 1:\n"
							  "    perf:\n"
							  "      min: text\n"
							  "      max: text\n";
		const auto system = angonoka::load_text(text);
		REQUIRE(system.agents[0].perf.mean() == Approx(1.f));
		REQUIRE(system.agents[0].perf.stddev() == Approx(1.5f));
	}

	SECTION("Parse performance")
	{
		constexpr auto text = "agents:\n"
							  "  agent 1:\n"
							  "    perf:\n"
							  "      min: 0.5\n"
							  "      max: 1.5\n"
							  "  agent 2:\n";
		const auto system = angonoka::load_text(text);
		REQUIRE(system.agents[0].perf.mean() == Approx(1.f));
		REQUIRE(system.agents[0].perf.stddev() == Approx(1.5f));
		REQUIRE(system.agents[1].perf.mean() == Approx(1.f));
		REQUIRE(system.agents[1].perf.stddev() == Approx(1.5f));
	}
}
