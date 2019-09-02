#include <catch2/catch.hpp>
#include <yaml-cpp/yaml.h>
#include <unordered_set>
#include <vector>
#include <string>
#include <string_view>

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

	System load_text(std::string_view text) {
		// text is a null-terminated string literal
		const auto node = YAML::Load(text.data());
		const auto agents = node["agents"];
		if(!agents)
			throw InvalidTasksDefinition{"Missing 'agents' section"};
		if(!agents.IsMap())
			throw InvalidTasksDefinition{"Section 'agents' has invalid type"};
		System system;
		for(auto&& agent: agents) {
			// fmt::print("{}\n", agent.first.Scalar());
		}
		return system;
	}
} // namespace angonoka

TEST_CASE("Loading yaml") {
	SECTION("No 'agents' section") {
		REQUIRE_THROWS_AS(angonoka::load_text(""), angonoka::InvalidTasksDefinition);
	}

	SECTION("Invalid agent specification") {
	constexpr auto text = R"_(
agents: 123
)_";
		REQUIRE_THROWS_AS(angonoka::load_text(text), angonoka::InvalidTasksDefinition);
	}

	auto system = angonoka::load_text(tasks_text);
}
