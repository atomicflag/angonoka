#pragma once

#include "../configuration.h"
#include <chrono>
#include <gsl/gsl-lite.hpp>
#include <string_view>
#include <utility>
#include <yaml-cpp/yaml.h>

namespace angonoka {
/**
    Load Configuration from a YAML string.

    @param text Null-terminated string

    @return An instance of Configuration
*/
Configuration load_text(gsl::czstring text);

/**
    Load Configuration from a YAML file.

    @param path YAML configuration location

    @return An instance of Configuration
*/
Configuration load_file(std::string_view path);
} // namespace angonoka

namespace angonoka::detail {
/**
    Finds or inserts a group into Configuration.groups.

    @param groups   An array of Groups
    @param group    Group name

    @return The index of the group in Configuration.groups
    and whether the insertion took place.
*/
std::pair<GroupIndex, bool>
find_or_insert_group(Groups& groups, std::string_view group);

/**
    Parses human-readable durations.

    Parses durations with resolution ranging from seconds
    up to and including months. Examples:

    1h 15min
    3 weeks and 5 months
    30 seconds

    @param text A string containing a duration.
    @return The duration in seconds.
*/
std::chrono::seconds parse_duration(std::string_view text);

/**
    Parses agents blocks.

    Parses blocks such as these:

    agents:
      agent 1:
      agent 2:

    @param node     "agents" node
    @param config   An instance of Configuration
*/
void parse_agents(const YAML::Node& node, Configuration& config);

/**
    Parses tasks blocks.

    Parses blocks such as these:

    tasks:
      - name: task 1
        duration:
          min: 1 h
          max: 3 h
      - name: task 2
        duration: 2h

    @param node     "tasks" node
    @param config   An instance of Configuration
*/
void parse_tasks(const YAML::Node& node, Configuration& config);
} // namespace angonoka::detail
