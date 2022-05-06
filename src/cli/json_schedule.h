#pragma once

#include "configuration.h"
#include "options.h"
#include "predict.h"
#include <nlohmann/json.hpp>
#include <string_view>

namespace angonoka::cli {
namespace detail {
    /**
        Convert a schedule to JSON.

        Example:

        {
            "makespan": 200,
            "tasks": [
                {
                    "task": "Do things",
                    "agent": "Bob",
                    "priority": 0,
                    "expected_duration": 100,
                    "expected_start": 0
                }
            ]
        }

        @param config   Tasks and agents
        @param schedule Optimized schedule

        @return JSON object
    */
    [[nodiscard]] nlohmann::json to_json(
        const Configuration& config,
        const OptimizedSchedule& schedule);
} // namespace detail

/**
    Optimize the schedule and output to JSON.

    @param config   Tasks and agents
    @param options  CLI options

    @return JSON object
*/
[[nodiscard]] nlohmann::json
json_schedule(const Configuration& config, const Options& options);

/**
    Save JSON to a file with formatting.

    Currently used for exporting the optimized schedule.

    TODO: Rename to save_schedule_json

    @param json     JSON to be saved
    @param options  CLI options
*/
void save_json(const nlohmann::json& json, const Options& options);
} // namespace angonoka::cli
