#include "cli/schedule.h"
#include "config/load.h"
#include <catch2/catch.hpp>

TEST_CASE("JSON schedule")
{
    using namespace angonoka;
    SECTION("basic schedule")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 1h";
        // clang-format on
        const auto config = load_text(text);

        const OptimizedSchedule schedule{
            .schedule{{.task_id{0}, .agent_id{0}}},
            .makespan{3600}};
        const auto json = cli::detail::to_json(config, schedule);

        REQUIRE(json["makespan"] == 3600);
        REQUIRE(json["tasks"].size() == 1);
        const auto& task = json["tasks"][0];
        REQUIRE(task["agent"] == "agent1");
        REQUIRE(task["task"] == "task 1");
        REQUIRE(task["priority"] == 0);
        REQUIRE(task["expected_duration"] == 3600);
        REQUIRE(task["expected_start"] == 0);
    }

    SECTION("advanced schedule")
    {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "  agent2:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 1h\n"
            "    id: A\n"
            "  - name: task 2\n"
            "    duration: 1h\n"
            "    depends_on: A\n"
            "    id: B\n"
            "  - name: task 3\n"
            "    duration: 1h\n"
            "    depends_on: B\n"
            "    id: C\n"
            "  - name: task 4\n"
            "    duration: 1h\n"
            "    depends_on: C";
        // clang-format on
        const auto config = load_text(text);

        const OptimizedSchedule schedule{
            .schedule{
                {.task_id{0}, .agent_id{0}},
                {.task_id{1}, .agent_id{1}},
                {.task_id{2}, .agent_id{1}},
                {.task_id{3}, .agent_id{0}}},
            .makespan{3600}};
        const auto json = cli::detail::to_json(config, schedule);

        REQUIRE(
            json
            == nlohmann::json{
                {"makespan", 3600},
                {"tasks",
                 {{{"task", "task 1"},
                   {"agent", "agent1"},
                   {"priority", 0},
                   {"expected_duration", 3600},
                   {"expected_start", 0}},
                  {{"task", "task 2"},
                   {"agent", "agent2"},
                   {"priority", 0},
                   {"expected_duration", 3600},
                   {"expected_start", 3600}},
                  {{"task", "task 3"},
                   {"agent", "agent2"},
                   {"priority", 1},
                   {"expected_duration", 3600},
                   {"expected_start", 7200}},
                  {{"task", "task 4"},
                   {"agent", "agent1"},
                   {"priority", 1},
                   {"expected_duration", 3600},
                   {"expected_start", 10800}}}}});
    }
}
