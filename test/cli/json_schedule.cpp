#include "cli/json_schedule.h"
#include "config/load.h"
#include <boost/ut.hpp>

using namespace boost::ut;

suite json_schedule = [] {
    using namespace angonoka;
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
        .makespan{3600},
        .schedule{{.task_id{0}, .agent_id{0}}}};
    const auto json = cli::detail::to_json(config, schedule);

    expect(json["makespan"] == 3600_i);
    expect(json["tasks"].size() == 1_i);
    const auto& task = json["tasks"][0];
    expect(task["agent"] == "agent1");
    expect(task["task"] == "task 1");
    expect(task["priority"] == 0_i);
    expect(task["expected_duration"] == 3600_i);
    // TODO: WIP: Implement
};
