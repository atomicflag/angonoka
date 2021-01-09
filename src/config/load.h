#pragma once

#include "../system.h"
#include <chrono>
#include <gsl/gsl-lite.hpp>
#include <string_view>
#include <utility>
#include <yaml-cpp/yaml.h>

namespace angonoka {
/**
    Load System from a YAML string.

    @param text Null-terminated string

    @return An instance of System
*/
System load_text(gsl::czstring text);
} // namespace angonoka

namespace angonoka::detail {
/**
    Finds or inserts a group into System.groups.

    @param sys    System instance
    @param groups An array of Groups

    @return The index of the group in System.groups
    and whether the insertion took place.
*/
std::pair<GroupId, bool>
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

    @param node   "agents" node
    @param sys    An instance of System
*/
void parse_agents(const YAML::Node& node, System& sys);

/**
    Parses tasks blocks.
    TODO: doc update

    Parses blocks such as these:

    tasks:
      task 1:
        days:
          min: 1
          max: 3
      task 2:
        days:
          min: 2
          max: 2

    @param node   "tasks" node
    @param sys    An instance of System
*/
void parse_tasks(const YAML::Node& node, System& sys);
} // namespace angonoka::detail
