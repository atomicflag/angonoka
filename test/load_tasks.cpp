#include "config/load.h"
#include "exceptions.h"
#include <boost/ut.hpp>

using namespace boost::ut;

#define ANGONOKA_COMMON_YAML                                         \
    "agents:\n"                                                      \
    "  agent1:\n"

suite loading_tasks = [] {
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    using namespace std::literals::string_view_literals;

    "no 'tasks' section"_test = [] {
        constexpr auto text = ANGONOKA_COMMON_YAML;
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "empty 'tasks' section"_test = [] {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "tasks:";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "empty array"_test = [] {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "tasks: []";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "invalid 'tasks' format"_test = [] {
        // clang-format off
        constexpr auto text = 
            ANGONOKA_COMMON_YAML
            "tasks: 123";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "a single task"_test = [] {
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
        expect(config.tasks.size() == 1_i);
        const auto& task = config.tasks[0];
        expect(task.name == "task 1");
        expect(!task.group_id);
        expect(task.duration.min == days{1});
        expect(task.duration.max == days{3});
    };

    "invalid task duration type"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration:\n"
            "      min: as\n"
            "      max: a";
        // clang-format on
        expect(throws<angonoka::InvalidDuration>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::InvalidDuration& e) {
                expect(eq(
                    e.what(),
                    R"(Task "task 1" has invalid duration "as".)"sv));
                throw;
            }
        }));
    };

    "invalid task duration"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration:\n"
            "      min: 5 days\n"
            "      max: 2 days";
        // clang-format on
        expect(throws<angonoka::TaskDurationMinMax>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::TaskDurationMinMax& e) {
                expect(eq(
                    e.what(),
                    R"(Task "task 1" has min duration that is greater than max duration.)"sv));
                throw;
            }
        }));
    };

    "missing task duration"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1";
        // clang-format on
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "exact duration"_test = [] {
        // clang-format off
        constexpr auto text =
            ANGONOKA_COMMON_YAML
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 3h";
        // clang-format on
        const auto config = angonoka::load_text(text);
        expect(config.tasks.size() == 1_i);
        const auto& task = config.tasks[0];
        expect(task.duration.min == 3h);
        expect(task.duration.max == 3h);
    };

    "valid group id"_test = [] {
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
        expect(config.tasks.size() == 1_i);
        const auto& task = config.tasks[0];
        // group_id is std::optional and has to have a value
        expect(static_cast<bool>(task.group_id));
        expect(task.group_id == 0);
    };

    "two tasks, one group"_test = [] {
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
        expect(config.tasks.size() == 2_i);
        const auto& task1 = config.tasks[0];
        const auto& task2 = config.tasks[1];
        expect(task1.group_id == 0);
        expect(task2.group_id == 0);
    };

    "duplicate tasks"_test = [] {
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
        expect(config.tasks.size() == 2_i);
        expect(config.tasks[0].name == "task 1");
        expect(config.tasks[1].name == "task 1");
    };

    "duplicate attributes"_test = [] {
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
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "no suitable agents"_test = [] {
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
        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "task with id"_test = [] {
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

        expect(config.tasks.size() == 1_i);
        expect(config.tasks[0].id == "task_1");
        expect(config.tasks[0].name == "task 1");
    };

    "empty id"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: 'task 1'\n"
            "    id: ''\n"
            "    duration: 1h";
        // clang-format on

        expect(throws<angonoka::CantBeEmpty>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::CantBeEmpty& e) {
                expect(eq(
                    e.what(),
                    R"(Task id for the task "task 1" can't be empty.)"sv));
                throw;
            }
        }));
    };

    "missing name"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - id: 'hello'\n"
            "    duration: 1h";
        // clang-format on

        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "empty name"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - id: 'hello'\n"
            "    name: ''\n"
            "    duration: 1h";
        // clang-format on

        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "optional id"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    duration: 1h";
        // clang-format on

        const auto config = angonoka::load_text(text);

        expect(config.tasks.size() == 1_i);
        expect(config.tasks[0].id.empty());
        expect(config.tasks[0].name == "task 1");
    };

    "dedicated agent"_test = [] {
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

        expect(config.tasks.size() == 1_i);
        expect(!config.tasks[0].group_id);
        expect(config.tasks[0].agent_id == AgentIndex{0});
        expect(can_work_on(config.agents[0], config.tasks[0]));
        expect(!can_work_on(config.agents[1], config.tasks[0]));
    };

    "dedicated agent and a group"_test = [] {
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

        expect(throws<angonoka::InvalidTaskAssignment>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::InvalidTaskAssignment& e) {
                expect(eq(
                    e.what(),
                    R"(Task "task 1" must have at most one of: agent, group.)"sv));
                throw;
            }
        }));
    };

    "dedicated agent and subtasks"_test = [] {
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

        expect(config.tasks.size() == 2_i);
        expect(config.tasks[0].agent_id == AgentIndex{0});
        expect(!config.tasks[1].agent_id);
    };

    "empty dedicated agent"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    agent: ''\n"
            "    duration: 1h";
        // clang-format on

        expect(throws<angonoka::CantBeEmpty>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::CantBeEmpty& e) {
                expect(
                    eq(e.what(),
                       R"(Assigned agents name can't be empty.)"sv));
                throw;
            }
        }));
    };

    "dedicated agent not found"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    agent: asdf\n"
            "    duration: 1h";
        // clang-format on

        expect(throws<angonoka::AgentNotFound>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::AgentNotFound& e) {
                expect(
                    eq(e.what(), R"(Agent "asdf" doesn't exist.)"sv));
                throw;
            }
        }));
    };

    "depends on single task"_test = [] {
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

        expect(config.tasks.size() == 2_i);
        expect(config.tasks[0].id == "A");
        expect(config.tasks[0].name == "task 1");
        expect(config.tasks[1].id.empty());
        expect(config.tasks[1].name == "task 2");
        expect(
            config.tasks[1].dependencies == angonoka::TaskIndices{0});
    };

    "empty dependency id"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: ''\n"
            "    duration: 1h";
        // clang-format on

        expect(throws<angonoka::CantBeEmpty>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::CantBeEmpty& e) {
                expect(eq(
                    e.what(),
                    R"(Dependency id of the task "task 1" can't be empty.)"sv));
                throw;
            }
        }));
    };

    "invalid dependency id"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    depends_on: A\n"
            "    duration: 1h";
        // clang-format on

        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "duplicate id"_test = [] {
        // clang-format off
        constexpr auto text =
            "agents:\n"
            "  agent1:\n"
            "tasks:\n"
            "  - name: task 1\n"
            "    id: A\n"
            "    duration: 1h\n"
            "  - name: task 2\n"
            "    id: A\n"
            "    duration: 1h";
        // clang-format on

        expect(throws<angonoka::DuplicateTaskDefinition>([&] {
            try {
                angonoka::load_text(text);
            } catch (const angonoka::DuplicateTaskDefinition& e) {
                expect(eq(e.what(), R"(Duplicate task id "A".)"sv));
                throw;
            }
        }));
    };

    "out of order dependencies"_test = [] {
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

        expect(config.tasks.size() == 2_i);
        expect(
            config.tasks[0].dependencies == angonoka::TaskIndices{1});
        expect(config.tasks[1].dependencies.empty());
    };

    "multiple dependencies"_test = [] {
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

        expect(config.tasks.size() == 3_i);
        expect(
            config.tasks[0].dependencies
            == angonoka::TaskIndices{1, 2});
        expect(config.tasks[1].dependencies.empty());
        expect(config.tasks[2].dependencies.empty());
    };

    "dependency cycle"_test = [] {
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

        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "depends on itself"_test = [] {
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

        expect(throws<angonoka::ValidationError>(
            [&] { angonoka::load_text(text); }));
    };

    "subtasks"_test = [] {
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

        expect(config.tasks.size() == 2_i);
        expect(config.tasks[0].name == "task 1");
        expect(
            config.tasks[0].dependencies == angonoka::TaskIndices{1});
        expect(config.tasks[1].name == "task 2");
        expect(config.tasks[1].dependencies.empty());
    };

    "deep subtasks"_test = [] {
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

        expect(config.tasks.size() == 7_i);
        expect(config.tasks[0].name == "task 1");
        expect(
            config.tasks[0].dependencies
            == angonoka::TaskIndices{1, 4});
        expect(config.tasks[1].name == "task 1.1");
        expect(
            config.tasks[1].dependencies
            == angonoka::TaskIndices{2, 3});
        expect(config.tasks[2].name == "task 1.1.1");
        expect(config.tasks[2].dependencies.empty());
        expect(config.tasks[3].name == "task 1.1.2");
        expect(config.tasks[3].dependencies.empty());
        expect(config.tasks[4].name == "task 1.2");
        expect(
            config.tasks[4].dependencies
            == angonoka::TaskIndices{5, 6});
        expect(config.tasks[5].name == "task 1.2.1");
        expect(config.tasks[5].dependencies.empty());
        expect(config.tasks[6].name == "task 1.2.2");
        expect(
            config.tasks[6].dependencies == angonoka::TaskIndices{3});
    };
};

#undef ANGONOKA_COMMON_YAML
