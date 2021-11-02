#include "config/load.h"
#include "exceptions.h"
#include <catch2/catch.hpp>

#define ANGONOKA_COMMON_YAML                                         \
    "agents:\n"                                                      \
    "  agent1:\n"

TEST_CASE("loading tasks")
{
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    using namespace std::literals::string_view_literals;
    using Catch::Message;

    SECTION("no 'tasks' section")
    {
        constexpr auto text = ANGONOKA_COMMON_YAML;

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("empty 'tasks' section")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "tasks:";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("empty array")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "tasks: []";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("invalid 'tasks' format")
    {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "tasks: 123";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("a single task")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 3 days";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 1);
        const auto& task = config.tasks[0];
        REQUIRE(task.name == "task 1");
        REQUIRE(task.group_ids.empty());
        REQUIRE(task.duration.min == days{1});
        REQUIRE(task.duration.max == days{3});
    }

    SECTION("invalid task duration type")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration:\n"
            "      min: as\n"
            "      max: a";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::InvalidDuration,
            Message(R"(Task "task 1" has invalid duration "as".)"));
    }

    SECTION("invalid task duration")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration:\n"
            "      min: 5 days\n"
            "      max: 2 days";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::TaskDurationMinMax,
            Message(
                R"(Task "task 1" has min duration that is greater than max duration.)"));
    }

    SECTION("missing task duration")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("exact duration")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 3h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 1);
        const auto& task = config.tasks[0];
        REQUIRE(task.duration.min == 3h);
        REQUIRE(task.duration.max == 3h);
    }

    SECTION("valid group id")
    {
        using angonoka::GroupIndices;
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    group: A\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 1);
        const auto& task = config.tasks[0];
        REQUIRE(task.group_ids.size() == 1);
        REQUIRE(task.group_ids == GroupIndices{0});
    }

    SECTION("multiple groups")
    {
        using angonoka::can_work_on;
        using angonoka::GroupIndices;
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "      - B\n"
            "      - C\n"
            "  agent2:\n"
            "    groups:\n"
            "      - A\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    groups:\n"
            "     - A\n"
            "     - B\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 1);
        const auto& task = config.tasks[0];
        REQUIRE(task.group_ids.size() == 2);
        REQUIRE(task.group_ids == GroupIndices{0, 1});
        REQUIRE(can_work_on(config.agents[0], task));
        REQUIRE_FALSE(can_work_on(config.agents[1], task));
    }

    SECTION("groups and group")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "      - B\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    group: A\n"
            "    groups:\n"
            "     - A\n"
            "     - B\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::InvalidTaskAssignment,
            Message(
                R"(Task "task 1" must have at most one of: agent, group, groups.)"));
    }

    SECTION("invalid groups format")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "      - B\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    groups: asdf\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("invalid group in groups")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "      - B\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    groups:\n"
            "      - C\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("two tasks, one group")
    {
        using angonoka::GroupIndices;
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    group: A\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days\n"
            "  - name: task 2\n"
            "    group: A\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 2);
        const auto& task1 = config.tasks[0];
        const auto& task2 = config.tasks[1];
        REQUIRE(task1.group_ids == GroupIndices{0});
        REQUIRE(task2.group_ids == GroupIndices{0});
    }

    SECTION("duplicate tasks")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days\n"
            "  - name: task 1\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 2);
        REQUIRE(config.tasks[0].name == "task 1");
        REQUIRE(config.tasks[1].name == "task 1");
    }

    SECTION("duplicate attributes")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "  agent2:\n"
            "    groups:\n"
            "      - B\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    group: A\n"
            "    group: B\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 3 days";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("no suitable agents")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    group: B\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 3 days";
        // clang-format on

        // "task 1" has a group "B" and the only agent can
        // only work on tasks from group "A".
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("task with id")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    id: task_1\n"
            "    duration: 1h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 1);
        REQUIRE(config.tasks[0].id == "task_1");
        REQUIRE(config.tasks[0].name == "task 1");
    }

    SECTION("empty id")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: 'task 1'\n"
            "    id: ''\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::CantBeEmpty,
            Message(
                R"(Task id for the task "task 1" can't be empty.)"));
    }

    SECTION("missing name")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - id: 'hello'\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("empty name")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - id: 'hello'\n"
            "    name: ''\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("optional id")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 1h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 1);
        REQUIRE(config.tasks[0].id.empty());
        REQUIRE(config.tasks[0].name == "task 1");
    }

    SECTION("dedicated agent")
    {
        using angonoka::AgentIndex;
        using angonoka::can_work_on;

        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "  agent2:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    agent: agent1\n"
            "    duration: 1h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 1);
        REQUIRE(config.tasks[0].group_ids.empty());
        REQUIRE(config.tasks[0].agent_id == AgentIndex{0});
        REQUIRE(can_work_on(config.agents[0], config.tasks[0]));
        REQUIRE_FALSE(can_work_on(config.agents[1], config.tasks[0]));
    }

    SECTION("dedicated agent and a group")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "    groups:\n"
            "      - A\n"
            "  agent2:\n"
            "    groups:\n"
            "      - A\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    agent: agent1\n"
            "    group: A\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::InvalidTaskAssignment,
            Message(
                R"(Task "task 1" must have at most one of: agent, group, groups.)"));
    }

    SECTION("dedicated agent and subtasks")
    {
        using angonoka::AgentIndex;

        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "  agent2:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    agent: agent1\n"
            "    duration: 1h\n"
            "    subtasks:\n"
            "      - name: task 2\n"
            "        duration: 2h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 2);
        REQUIRE(config.tasks[0].agent_id == AgentIndex{0});
        REQUIRE_FALSE(config.tasks[1].agent_id);
    }

    SECTION("empty dedicated agent")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    agent: ''\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::CantBeEmpty,
            Message(R"(Assigned agents name can't be empty.)"));
    }

    SECTION("dedicated agent not found")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    agent: asdf\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::AgentNotFound,
            Message(R"(Agent "asdf" doesn't exist.)"));
    }

    SECTION("depends on single task")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    id: A\n"
            "    duration: 1h\n"
            "  - name: task 2\n"
            "    depends_on: A\n"
            "    duration: 2h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 2);
        REQUIRE(config.tasks[0].id == "A");
        REQUIRE(config.tasks[0].name == "task 1");
        REQUIRE(config.tasks[1].id.empty());
        REQUIRE(config.tasks[1].name == "task 2");
        REQUIRE(
            config.tasks[1].dependencies == angonoka::TaskIndices{0});
    }

    SECTION("empty dependency id")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: ''\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::CantBeEmpty,
            Message(
                R"(Dependency id of the task "task 1" can't be empty.)"));
    }

    SECTION("invalid dependency id")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: A\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("duplicate id")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    id: A\n"
            "    duration: 1h\n"
            "  - name: task 2\n"
            "    id: A\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_MATCHES(
            angonoka::load_text(text),
            angonoka::DuplicateTaskDefinition,
            Message(R"(Duplicate task id "A".)"));
    }

    SECTION("out of order dependencies")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: B\n"
            "    duration: 1h\n"
            "  - name: task 2\n"
            "    id: B\n"
            "    duration: 2h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 2);
        REQUIRE(
            config.tasks[0].dependencies == angonoka::TaskIndices{1});
        REQUIRE(config.tasks[1].dependencies.empty());
    }

    SECTION("multiple dependencies")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on:\n"
            "      - A\n"
            "      - B\n"
            "    duration: 1h\n"
            "  - name: task 2\n"
            "    id: B\n"
            "    duration: 2h\n"
            "  - name: task 3\n"
            "    id: A\n"
            "    duration: 3h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 3);
        REQUIRE(
            config.tasks[0].dependencies
            == angonoka::TaskIndices{1, 2});
        REQUIRE(config.tasks[1].dependencies.empty());
        REQUIRE(config.tasks[2].dependencies.empty());
    }

    SECTION("dependency cycle")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: B\n"
            "    id: A\n"
            "    duration: 1h\n"
            "  - name: task 1\n"
            "    depends_on: C\n"
            "    id: B\n"
            "    duration: 1h\n"
            "  - name: task 2\n"
            "    id: C\n"
            "    depends_on: A\n"
            "    duration: 3h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("depends on itself")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: A\n"
            "    id: A\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("subtasks")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 1h\n"
            "    subtasks:\n"
            "      - name: task 2\n"
            "        duration: 2h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 2);
        REQUIRE(config.tasks[0].name == "task 1");
        REQUIRE(
            config.tasks[0].dependencies == angonoka::TaskIndices{1});
        REQUIRE(config.tasks[1].name == "task 2");
        REQUIRE(config.tasks[1].dependencies.empty());
    }

    SECTION("deep subtasks")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 1h\n"
            "    subtasks:\n"
            "      - name: task 1.1\n"
            "        duration: 2h\n"
            "        subtasks:\n"
            "          - name: task 1.1.1\n"
            "            duration: 2h\n"
            "          - name: task 1.1.2\n"
            "            id: X\n"
            "            duration: 2h\n"
            "      - name: task 1.2\n"
            "        duration: 2h\n"
            "        subtasks:\n"
            "          - name: task 1.2.1\n"
            "            duration: 2h\n"
            "          - name: task 1.2.2\n"
            "            depends_on: X\n"
            "            duration: 2h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 7);
        REQUIRE(config.tasks[0].name == "task 1");
        REQUIRE(
            config.tasks[0].dependencies
            == angonoka::TaskIndices{1, 4});
        REQUIRE(config.tasks[1].name == "task 1.1");
        REQUIRE(
            config.tasks[1].dependencies
            == angonoka::TaskIndices{2, 3});
        REQUIRE(config.tasks[2].name == "task 1.1.1");
        REQUIRE(config.tasks[2].dependencies.empty());
        REQUIRE(config.tasks[3].name == "task 1.1.2");
        REQUIRE(config.tasks[3].dependencies.empty());
        REQUIRE(config.tasks[4].name == "task 1.2");
        REQUIRE(
            config.tasks[4].dependencies
            == angonoka::TaskIndices{5, 6});
        REQUIRE(config.tasks[5].name == "task 1.2.1");
        REQUIRE(config.tasks[5].dependencies.empty());
        REQUIRE(config.tasks[6].name == "task 1.2.2");
        REQUIRE(
            config.tasks[6].dependencies == angonoka::TaskIndices{3});
    }
}

#undef ANGONOKA_COMMON_YAML
