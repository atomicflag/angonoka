#include <catch2/catch.hpp>

#include <yaml-cpp/yaml.h>
#include <unordered_set>
#include <vector>
#include <string>
#include <string_view>
#include <range/v3/algorithm/find.hpp>
#include <fmt/format.h>
#include <boost/safe_numerics/safe_integer.hpp>

#include <fmt/printf.h>

namespace  {
	constexpr auto tasks_text = R"_(
agents:
  agent 1:
    groups:
      - A
      - B
  agent 2:
    multiplier: 0.8
  agent 3:
    multiplier: 0.5
    groups:
      - C
)_";
} // namespace 

namespace angonoka {
	using GroupIds = std::unordered_set<int>;

	struct Agent {
		GroupIds group_ids;
		std::string name;
	};

	using Groups = std::vector<std::string>;
	using Agents = std::vector<Agent>;

	struct System {
		Groups groups;
		Agents agents;
	};

	struct InvalidTasksDefinition : std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	using Int = boost::safe_numerics::safe<int>;

	namespace detail {
		Int find_or_insert_group(System& sys, const YAML::Node& g) {
			if(const auto& f = ranges::find(sys.groups, g.Scalar());
				f != sys.groups.end()) {
				return std::distance(sys.groups.begin(), f);
			}
			sys.groups.emplace_back(g.Scalar());
			return sys.groups.size()-1;
		}
	} // namespace detail

	System load_text(const char* text) {
		const auto node = YAML::Load(text);
		const auto agents = node["agents"];
		if(!agents)
			throw InvalidTasksDefinition{
				"Missing 'agents' section"};
		if(!agents.IsMap())
			throw InvalidTasksDefinition{
				"Section 'agents' has an invalid type"};
		System system;
		for(auto&& agent: agents) {
			if(!agent.second.IsMap())
				throw InvalidTasksDefinition{
					fmt::format("Invalid agent specification for '{}'", agent.first)};
			auto& a = system.agents.emplace_back();

			// Parse agent.name
			a.name = agent.first.Scalar();

			// Parse agent.groups
			if(const auto groups = agent.second["groups"]; groups) {
				if(!groups.IsSequence())
					throw InvalidTasksDefinition{
						fmt::format("Invalid groups specification for '{}'", a.name)};
				for(auto&& g : groups) {
					const auto gid = detail::find_or_insert_group(system, g);
					a.group_ids.emplace(gid);
				}
			}
		}
		return system;
	}
} // namespace angonoka

TEST_CASE("Loading yaml") {
	SECTION("No 'agents' section") {
		REQUIRE_THROWS_AS(angonoka::load_text(""), angonoka::InvalidTasksDefinition);
	}

	SECTION("Section 'agents' has an invalid type") {
		constexpr auto text = "agents: 123";
		REQUIRE_THROWS_AS(angonoka::load_text(text), angonoka::InvalidTasksDefinition);
	}

	SECTION("Invalid agent spec") {
		constexpr auto text =
			"agents:\n"
			"  agent 1: 123";
		REQUIRE_THROWS_AS(angonoka::load_text(text), angonoka::InvalidTasksDefinition);
	}

	SECTION("Invalid group spec") {
		constexpr auto text =
			"agents:\n"
			"  agent 1:\n"
			"    groups: 123";
		REQUIRE_THROWS_AS(angonoka::load_text(text), angonoka::InvalidTasksDefinition);
	}

	SECTION("Parse groups") {
		constexpr auto text =
			"agents:\n"
			"  agent 1:\n"
			"    groups:\n"
			"      - A\n"
			"      - B\n"
			"  agent 2:\n"
			"    groups:\n"
			"      - A\n"
			"      - C\n";
		const auto system = angonoka::load_text(text);
		REQUIRE(system.groups == angonoka::Groups{"A", "B", "C"});
		REQUIRE(system.agents.size() == 2);
		// Agent 1 has A(0) and B(1)
		REQUIRE(system.agents[0].group_ids == angonoka::GroupIds{0,1});
		// Agent 2 has A(0) and C(2)
		REQUIRE(system.agents[1].group_ids == angonoka::GroupIds{0,2});
	}

	auto system = angonoka::load_text(tasks_text);
}
