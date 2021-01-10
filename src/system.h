#pragma once

#include "common.h"
#include <boost/container/flat_set.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace angonoka {
using boost::container::flat_set;
using GroupId = int8;
using TaskId = int8;
using GroupIds = flat_set<GroupId>;
using TaskIds = flat_set<TaskId>;

/**
    Agent that performs Tasks.

    Agent::group_ids refer to Group indices in the System::groups. The
    lower the performance parameter the slower an agent will
    perform any given task. An agent can only perform tasks belonging
    to groups in group_ids.

    @var name           Agent's name
    @var group_ids      flat_set of Group ids
    @var performance    Performance min/max
*/
struct Agent {
    std::string name;
    GroupIds group_ids;
    struct Performance {
        static constexpr float default_min = .5F;
        static constexpr float default_max = 1.5F;
        float min = default_min;
        float max = default_max;
    };
    Performance performance;
};

/**
    Tells if the agent can work on any task.

    @param agent Agent

    @return True if the agent can perform any task.
*/
[[nodiscard]] bool is_universal(const Agent& agent) noexcept;

/**
    Checks if the agent can work on tasks from a given group.

    @param agent Agent
    @param id Group id

    @return True if the agent can work with a given group.
*/
[[nodiscard]] bool
can_work_on(const Agent& agent, GroupId id) noexcept;

/**
    Task performed by an Agent.

    If a Task has a Group, it will be processed by Agents that can
    perform Tasks from that group. Otherwise it will be processed by
    any Agent.

    TODO: update doc

    @var name     Task's name
    @var group_id Group id, if any
    @var dur      Duration min/max in seconds
*/
struct Task {
    std::string name;
    std::string id;
    std::optional<GroupId> group_id;
    TaskIds dependencies;
    struct Duration {
        std::chrono::seconds min, max;
    };
    Duration duration;
};

using Groups = std::vector<std::string>;
using Agents = std::vector<Agent>;
using Tasks = std::vector<Task>;

/**
    System that represents Tasks and Agents.

    @var groups   Task groups
    @var agents   Agents that perform tasks
    @var tasks    All of the tasks
*/
struct System {
    Groups groups;
    Agents agents;
    Tasks tasks;
};

/**
    Checks if any of the agents are "universal".

    A "universal" agent is an agent that can perform any task.

    @param system System

    @return True if there is at least 1 universal agent.
*/
[[nodiscard]] bool
has_universal_agents(const System& system) noexcept;
} // namespace angonoka
