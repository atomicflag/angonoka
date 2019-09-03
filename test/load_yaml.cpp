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
#include <random>

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
    groups:
      - C
)_";
} // namespace 

namespace angonoka {
	using GroupIds = std::unordered_set<int>;
	using Normal = std::normal_distribution<float>;

	struct Agent {
		std::string name;
		GroupIds group_ids;
		Normal perf;
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
		/**
			Create mean and stddev from min and max values.

			Mean is an average of min and max.

			mean = (min+max) / 2

			Stddev is the difference between the mean and min/max
			multiplied by stdnum.

			stddev = (max-mean)*stdnum

			\param min Lower bound
			\param max Upper bound
			\param stdnum Size of stddev (1 by default)
			\return A tuple with mean and stddev
		*/
		std::tuple<float, float> make_normal_params(
			float min,
			float max,
			float stdnum=1.f) {
			const float mean = (min+max)/2.f;
			return {mean, (max-mean)*stdnum};
		}

		/**
			Finds or inserts a group into System.groups.

			\param sys System instance
			\param group Group name
			\return Index of the group in System.groups
		*/
		Int find_or_insert_group(System& sys, std::string_view group) {
			if(const auto& f = ranges::find(sys.groups, group);
				f != sys.groups.end()) {
				return std::distance(sys.groups.begin(), f);
			}
			sys.groups.emplace_back(group);
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

		/**
			Parses agent groups section.

			Parses blocks such as these:

			groups:
				- A
				- B
				- C

			and inserts "A", "B", "C" into System.groups.
			Then places group ids into agent.groups_ids.

			\param groups Sequence with group names
			\param agent An instance of Agent
			\param sys An instance of System
		*/
		void parse_agent_groups(
			const YAML::Node& groups,
			Agent& agent,
			System& sys) {
			using detail::find_or_insert_group;
			for(auto&& g : groups) {
				const auto gid = find_or_insert_group(sys, g.Scalar());
				agent.group_ids.emplace(gid);
			}
		}

		void validate_agent(
			const YAML::Node& agent,
			const YAML::Node& agent_data) {
			if(agent_data.IsSequence() ||
				agent_data.IsScalar() ||
				!agent_data.IsDefined())
				throw InvalidTasksDefinition{
					fmt::format("Invalid agent"
						" specification for \"{}\"", agent)};
		}

		void validate_agent_perf(
			const YAML::Node& perf,
			Agent& agent) {
			if(!perf.IsMap() || !perf["min"]  || !perf["max"])
				throw InvalidTasksDefinition{
					fmt::format("Invalid perf specification"
						" for \"{}\"", agent.name)};
		}

		/**
			Parses agent perf.

			Parses blocks such as these:
			
			perf:
				min: 1.0
				max: 2.0

			\param perf Map with perf data
			\param agent An instance of Agent
		*/
		void parse_agent_perf(
			const YAML::Node& perf,
			Agent& agent) {
			const auto [mean, stddev] = make_normal_params(
				perf["min"].as<float>(.5f),
				perf["max"].as<float>(1.5f),
				3.f);
			agent.perf = Normal{mean, stddev};
		}

		/**
			Assigns a default performance values.

			Currently the default is Normal(1, 1.5)

			\param agent An instance of Agent
		*/
		void assign_default_perf(Agent& agent) {
			agent.perf = Normal{1.f, 1.5f};
		}

		/**
			Parses agent blocks.

			Parses blocks such as these:

			agent 1:
				perf:
					min: 0.5
					max: 1.5
				groups:
					- A
					- B

			\param agent_node Scalar holding the name of the agent
			\param agent_data Map with agent data
			\param sys An instance of System
		*/
		void parse_agent(
			const YAML::Node& agent_node,
			const YAML::Node& agent_data,
			System& sys) {
			auto& agent = sys.agents.emplace_back();

			// Parse agent.name
			agent.name = agent_node.Scalar();

			validate_agent(agent_node, agent_data);

			// Parse agent.groups
			if(const auto groups = agent_data["groups"]) {
				validate_agent_groups(groups, agent);
				parse_agent_groups(groups, agent, sys);
			}

			// Parse agent.perf
			if(const auto perf = agent_data["perf"]) {
				validate_agent_perf(perf, agent);
				parse_agent_perf(perf, agent);
			} else {
				assign_default_perf(agent);
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

			"agent 2" will implicitly have groups "A" and "B"

			\param sys An instance of System
		*/
		void fill_empty_groups(System& sys) {
			using ranges::to;
			using namespace ranges::views;
			for(auto&& a : sys.agents) {
				if(!a.group_ids.empty()) continue;
				a.group_ids = to<GroupIds>(iota(0,
					static_cast<int>(sys.groups.size())));
			}
		}

		/**
			Parses agents blocks.

			Parses blocks such as these:

			agents:
				agent 1:
				agent 2:

			\param node "agents" node
			\param sys An instance of System
		*/
		void parse_agents(const YAML::Node& node, System& sys) {
			for(auto&& agent: node) {
				parse_agent(agent.first, agent.second, sys);
			}
		}
	} // namespace detail

	/**
		Load System from a YAML string.

		\param text Null-terminated string
		\return An instance of System
	*/
	System load_text(const char* text) {
		const auto node = YAML::Load(text);
		System system;
		const auto agents = node["agents"];
		detail::validate_agents(agents);
		detail::parse_agents(agents, system);
		if(!system.groups.empty())
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

	SECTION("No groups") {
		constexpr auto text =
			"agents:\n"
			"  agent 1:\n"
			"  agent 2:";
		const auto system = angonoka::load_text(text);
		REQUIRE(system.agents[0].group_ids.empty());
		REQUIRE(system.agents[1].group_ids.empty());
	}

	SECTION("Invalid perf section") {
		constexpr auto text =
			"agents:\n"
			"  agent 1:\n"
			"    perf:\n";
		REQUIRE_THROWS_AS(
			angonoka::load_text(text),
			angonoka::InvalidTasksDefinition);
	}

	SECTION("Missing perf value") {
		constexpr auto text =
			"agents:\n"
			"  agent 1:\n"
			"    perf:\n"
			"      min: 1.0\n";
		REQUIRE_THROWS_AS(
			angonoka::load_text(text),
			angonoka::InvalidTasksDefinition);
	}

	SECTION("Invalid perf type") {
		constexpr auto text =
			"agents:\n"
			"  agent 1:\n"
			"    perf:\n"
			"      min: text\n"
			"      max: text\n";
		const auto system = angonoka::load_text(text);
		REQUIRE(system.agents[0].perf.mean() == Approx(1.f));
		REQUIRE(system.agents[0].perf.stddev() == Approx(1.5f));
	}

	SECTION("Parse performance") {
		constexpr auto text =
			"agents:\n"
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

	auto system = angonoka::load_text(tasks_text);
}
