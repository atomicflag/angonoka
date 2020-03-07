#pragma once

#include "../system.h"
#include <string_view>
#include <utility>
#include <yaml-cpp/yaml.h>

namespace angonoka {
/**
  Load System from a YAML string.

  @param text Null-terminated string

  @return An instance of System
*/
System load_text(const char* text);
} // namespace angonoka

namespace angonoka::detail {
/**
  Finds or inserts a group into System.groups.

  @param sys    System instance
  @param groups An array of Groups

  @return A pair of the index of the group in System.groups
  and whether the insert took place.
*/
std::pair<int, bool>
find_or_insert_group(Groups& groups, std::string_view group);

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
