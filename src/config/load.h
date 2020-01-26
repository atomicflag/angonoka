#pragma once

#include "../system.h"
#include <string_view>
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
  Fills empty agent.group_ids with all group ids.

  E.g.

  agents:
    agent 1:
      groups:
        - A
        - B
    agent 2:

  "agent 2" will implicitly have groups "A" and "B"

  @param sys An instance of System
*/
void fill_empty_groups(System& sys);

/**
  Finds or inserts a group into System.groups.

  @param sys    System instance
  @param groups An array of Groups

  @return Index of the group in System.groups
*/
int find_or_insert_group(Groups& groups, std::string_view group);

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
