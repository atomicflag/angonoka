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
            "  task 1:\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 3 days";
        // clang-format on
        const auto system = angonoka::load_text(text);
        REQUIRE(system.tasks.size() == 1);
        const auto& task = system.tasks[0];
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
            "  task 1:\n"
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
            "  task 1:\n"
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
            "  task 1:";
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
            "  task 1:\n"
            "    duration: 3h";
        // clang-format on
        const auto system = angonoka::load_text(text);
        REQUIRE(system.tasks.size() == 1);
        const auto& task = system.tasks[0];
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
            "  task 1:\n"
            "    group: A\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on
        const auto system = angonoka::load_text(text);
        REQUIRE(system.tasks.size() == 1);
        const auto& task = system.tasks[0];
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
            "  task 1:\n"
            "    group: A\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days\n"
            "  task 2:\n"
            "    group: A\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on
        const auto system = angonoka::load_text(text);
        REQUIRE(system.tasks.size() == 2);
        const auto& task1 = system.tasks[0];
        const auto& task2 = system.tasks[1];
        REQUIRE(task1.group_id == 0);
        REQUIRE(task2.group_id == 0);
    }

    SECTION("Duplicate tasks")
    {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  task 1:\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days\n"
            "  task 1:\n"
            "    duration:\n"
            "      min: 1 day\n"
            "      max: 2 days";
        // clang-format on
        REQUIRE_THROWS_AS(
            angonoka::load_text(text),
            angonoka::ValidationError);
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
            "  task 1:\n"
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
            "  task 1:\n"
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

    SECTION("Task list")
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

        const auto system = angonoka::load_text(text);

        REQUIRE(system.tasks.size() == 1);
        REQUIRE(system.tasks[0].id == "task_1");
        REQUIRE(system.tasks[0].name == "task 1");
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

        const auto system = angonoka::load_text(text);

        REQUIRE(system.tasks.size() == 1);
        REQUIRE(system.tasks[0].id.empty());
        REQUIRE(system.tasks[0].name == "task 1");
    }
}

#undef ANGONOKA_COMMON_YAML
