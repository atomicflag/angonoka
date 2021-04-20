#include "config/load.h"
#include "exceptions.h"
#include <catch2/catch.hpp>

#define ANGONOKA_COMMON_YAML                                         \
    "agents:\n"                                                      \
    "  agent1:\n"

TEST_CASE("Loading tasks")
{
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;

    SECTION("No 'tasks' section")
    {
        constexpr auto text = ANGONOKA_COMMON_YAML;
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Empty 'tasks' section")
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

    SECTION("Invalid 'tasks' format")
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

    SECTION("A single task")
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
        REQUIRE_FALSE(task.group_id);
        REQUIRE(task.duration.min == days{1});
        REQUIRE(task.duration.max == days{3});
    }

    SECTION("Invalid task duration type")
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
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Invalid task duration")
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
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Missing task duration")
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

    SECTION("Exact duration")
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

    SECTION("Valid group id")
    {
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
        // group_id is std::optional and has to have a value
        REQUIRE(task.group_id);
        REQUIRE(task.group_id == 0);
    }

    SECTION("Two tasks, one group")
    {
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
        REQUIRE(task1.group_id == 0);
        REQUIRE(task2.group_id == 0);
    }

    SECTION("Duplicate tasks")
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

    SECTION("Duplicate attributes")
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

    SECTION("No suitable agents")
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

    SECTION("Task with id")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
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

    SECTION("Empty id")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: 'task 1'\n"
            "    id: ''\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Missing name")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - id: 'hello'\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Optional id")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 1h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        REQUIRE(config.tasks.size() == 1);
        REQUIRE(config.tasks[0].id.empty());
        REQUIRE(config.tasks[0].name == "task 1");
    }

    SECTION("Depends on single task")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
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

    SECTION("Empty dependency id")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: ''\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Invalid dependency id")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: A\n"
            "    duration: 1h";
        // clang-format on

        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
    }

    SECTION("Out of order dependencies")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
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

    SECTION("Multiple dependencies")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
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

    SECTION("Dependency cycle")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
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

    SECTION("Depends on itself")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
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

    SECTION("Subtasks")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
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

    SECTION("Deep subtasks")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
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
