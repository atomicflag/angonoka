#include <catch2/catch.hpp>

#include <yaml-cpp/yaml.h>
#include <unordered_set>
#include <vector>
#include <string>
#include <string_view>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/range/conversion.hpp>
#include <fmt/format.h>
#include <boost/safe_numerics/safe_integer.hpp>

#include <fmt/printf.h>

namespace  {
	constexpr auto tasks_text = R"_(
agents:
  agent 1:
    perf:
      min: 0.5
      max: 1.0
    groups:
      - A
      - B
  agent 2:
    perf:
      min: 0.5
      max: 1.0
  agent 3:
    perf:
      min: 0.3
      max: 0.7
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
	using ranges::to;
	using namespace ranges::views;

	namespace detail {
		Int find_or_insert_group(System& sys, const YAML::Node& g) {
			if(const auto& f = ranges::find(sys.groups, g.Scalar());
				f != sys.groups.end()) {
				return std::distance(sys.groups.begin(), f);
			}
			sys.groups.emplace_back(g.Scalar());
			return sys.groups.size()-1;
		}

		void validate_agents(const YAML::Node& node) {
			if(!node)
				throw InvalidTasksDefinition{
					"Missing \"agents\" section"};
			if(!node.IsMap())
				throw InvalidTasksDefinition{
					"Section \"agents\" has an invalid type"};
		}

		void validate_agent_groups(
			const YAML::Node& groups,
			Agent& agent) {
			if(!groups.IsSequence())
				throw InvalidTasksDefinition{
					fmt::format("Invalid groups specification"
						" for \"{}\"", agent.name)};
		}

		void parse_agent_groups(
			const YAML::Node& groups,
			Agent& agent,
			System& sys) {
			for(auto&& g : groups) {
				const auto gid = detail::find_or_insert_group(sys, g);
				agent.group_ids.emplace(gid);
			}
		}

		void validate_agent(
			const YAML::Node& agent,
			const YAML::Node& agent_data) {
			if(!agent_data.IsMap())
				throw InvalidTasksDefinition{
					fmt::format("Invalid agent"
						" specification for \"{}\"", agent)};
		}

		void add_agent(
			const YAML::Node& agent,
			const YAML::Node& agent_data,
			System& sys) {
			auto& a = sys.agents.emplace_back();

			// Parse agent.name
			a.name = agent.Scalar();

			if(agent_data.IsNull()) return;
			validate_agent(agent, agent_data);

			// Parse agent.groups
			if(const auto groups = agent_data["groups"]) {
				validate_agent_groups(groups, a);
				parse_agent_groups(groups, a, sys);
			}
		}

		/**
			Fills agent.group_ids with all available group ids
			when it wasn't specified in YAML.

			E.g.

			agents:
				agent 1:
					groups:
						- A
						- B
				agent 2:

			"agent 2" should implicitly have groups "A" and "B".
		*/
		void fill_empty_groups(System& sys) {
			for(auto&& a : sys.agents) {
				if(!a.group_ids.empty()) continue;
				a.group_ids = to<GroupIds>(iota(0,
					static_cast<int>(sys.groups.size())));
			}
		}

		void parse_agents(const YAML::Node& node, System& sys) {
			validate_agents(node);
			for(auto&& agent: node) {
				add_agent(agent.first, agent.second, sys);
			}
		}
	} // namespace detail

	/**
		Load System from a YAML string.

		\param text Null-terminated string
	*/
	System load_text(const char* text) {
		const auto node = YAML::Load(text);
		System system;
		detail::parse_agents(node["agents"], system);
		detail::fill_empty_groups(system);
		return system;
	}
} // namespace angonoka

TEST_CASE("Loading yaml") {
	SECTION("No 'agents' section") {
		REQUIRE_THROWS_AS(
			angonoka::load_text(""),
			angonoka::InvalidTasksDefinition);
	}

	SECTION("Section 'agents' has an invalid type") {
		constexpr auto text = "agents: 123";
		REQUIRE_THROWS_AS(
			angonoka::load_text(text),
			angonoka::InvalidTasksDefinition);
	}

	SECTION("Invalid agent spec") {
		constexpr auto text =
			"agents:\n"
			"  agent 1: 123";
		REQUIRE_THROWS_AS(
			angonoka::load_text(text),
			angonoka::InvalidTasksDefinition);
	}

	SECTION("Invalid group spec") {
		constexpr auto text =
			"agents:\n"
			"  agent 1:\n"
			"    groups: 123";
		REQUIRE_THROWS_AS(
			angonoka::load_text(text),
			angonoka::InvalidTasksDefinition);
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

	SECTION("Fill empty groups") {
		constexpr auto text =
			"agents:\n"
			"  agent 1:\n"
			"    groups:\n"
			"      - A\n"
			"      - B\n"
			"  agent 2:";
		const auto system = angonoka::load_text(text);
		// Agent 1 has A(0) and B(1)
		REQUIRE(system.agents[0].group_ids == angonoka::GroupIds{0,1});
		// Agent 2 should have all groups
		REQUIRE(system.agents[1].group_ids == angonoka::GroupIds{0,1});
	}

	auto system = angonoka::load_text(tasks_text);
}
