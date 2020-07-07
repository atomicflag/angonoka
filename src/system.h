#pragma once

#include "common.h"
#include <chrono>
#include <optional>
#include <string>

namespace angonoka {
using GroupId = int8;
constexpr auto static_alloc_group_ids = 5;
using GroupIds = Set<GroupId, static_alloc_group_ids>;

/**
    Agent that performs Tasks.

    Agent::group_ids refer to Group indices in the System::groups. The
    lower the performance parameter is the slower an agent will
    perform any given task. An agent can only perform tasks belonging
    to groups in group_ids.

    @var name           Agent's name
    @var group_ids      Set of Group ids
    @var performance    Performance min/max
*/
// NOLINTNEXTLINE(bugprone-exception-escape)
struct Agent {
    std::string name;
    GroupIds group_ids;
    struct Performance {
        /**
            Validated performance value.
        */
        class Value {
        public:
            Value(float v);
            operator float() const noexcept;

        private:
            float value;
        };
        static constexpr float default_min = .5F;
        static constexpr float default_max = 1.5F;
        Value min = default_min;
        Value max = default_max;
    };
    Performance performance;

    /**
        Tells if the agent can work on any task.

        @return True if the agent can perform any task.
    */
    [[nodiscard]] bool is_universal() const noexcept;

    /**
        Checks if the agent can work on tasks from a given group.

        @param id Group id

        @return True if the agent can work with a given group.
    */
    [[nodiscard]] bool can_work_on(GroupId id) const noexcept;
};

/**
    Task performed by an Agent.

    If a Task has a Group, it will be processed by Agents that can
    perform Tasks from that group. Otherwise it will be processed by
    any Agent.

    @var name     Task's name
    @var group_id Group id, if any
    @var dur      Duration min/max in seconds
*/
struct Task {
    std::string name;
    std::optional<GroupId> group_id;
    struct Duration {
        std::chrono::seconds min, max;
    };
    Duration duration;
};

constexpr auto static_alloc_groups = 5;
using Groups = Vector<std::string, static_alloc_groups>;
constexpr auto static_alloc_agents = 5;
using Agents = Vector<Agent, static_alloc_agents>;
constexpr auto static_alloc_tasks = 7;
using Tasks = Vector<Task, static_alloc_tasks>;

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

    /**
        Checks if any of the agents are "universal".

        A "universal" agent is an agent that can perform any task.

        @return True if there is at least 1 universal agent.
    */
    // NOLINTNEXTLINE(bugprone-exception-escape)
    [[nodiscard]] bool has_universal_agents() const noexcept;
};
} // namespace angonoka
