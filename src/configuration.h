#pragma once

#include "common.h"
#include <boost/container/flat_set.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace angonoka {
using boost::container::flat_set;
using GroupIndex = int8;
using TaskIndex = int8;
using AgentIndex = int8;
using GroupIndices = flat_set<GroupIndex>;
using TaskIndices = flat_set<TaskIndex>;

/**
    Agent that performs Tasks.

    Agent::group_ids refer to Group indices in the
    Configuration::groups. The lower the performance parameter the
    slower an agent will perform any given task. An agent can only
    perform tasks belonging to groups in group_ids.

    @var name           Agent's name
    @var group_ids      flat_set of Group ids
    @var performance    Performance min/max
*/
struct Agent {
    AgentIndex id;
    std::string name;
    GroupIndices group_ids;
    struct Performance {
        static constexpr float default_performance = 1.F;
        float min = default_performance;
        float max = default_performance;

        /**
            Calculates average (expected) performance.

            @return Expected performance.
        */
        [[nodiscard]] float average() const;
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
can_work_on(const Agent& agent, GroupIndex id) noexcept;

/**
    Task performed by an Agent.

    If a task has a group, it will be processed by agents that can
    perform tasks from that group. Otherwise it will be processed by
    any agent.

    If a task has multiple groups, the agent has to belong to all
    of tasks groups in order to be assignable to this task.

    @var name           Task's name
    @var id             Task's unique id
    @var group_ids      IDs of groups this task belongs to
    @var dependencies   Task prerequisites
    @var duration       Duration min/max in seconds
*/
struct Task {
    std::string name;
    std::string id;
    GroupIndices group_ids;
    std::optional<AgentIndex> agent_id;
    TaskIndices dependencies;
    struct Duration {
        std::chrono::seconds min, max;
        /**
            Calculates average (expected) duration.

            @return Expected duration in seconds.
        */
        [[nodiscard]] std::chrono::seconds average() const;
    };
    Duration duration;
};

/**
    Checks if the agent can work on a given task.

    @param agent Agent
    @param task Task

    @return True if the agent can work on this task.
*/
[[nodiscard]] bool
can_work_on(const Agent& agent, const Task& task) noexcept;

using Groups = std::vector<std::string>;
using Agents = std::vector<Agent>;
using Tasks = std::vector<Task>;

/**
    Parameters to pass to the schedule optimization algorithm.

    See respective definitions in the source for more details.

    Note: For performance reasons, restart_period must be
    a power of 2.

    @var batch_size     Number of STUN iterations in each update
    @var max_idle_iters Halting condition
    @var beta_scale     Temperature parameter's inertia
    @var stun_window    Temperature adjustment window
    @var gamma          Domain-specific parameter for STUN
    @var restart_period Temperature volatility period
*/
struct OptimizationParameters {
    static constexpr auto default_batch_size{10'000};
    int32 batch_size{default_batch_size};

    static constexpr auto default_max_idle_iters{100'000};
    int32 max_idle_iters{default_max_idle_iters};

    static constexpr auto default_beta_scale{1e-3F};
    float beta_scale{default_beta_scale};

    static constexpr auto default_stun_window{100};
    int32 stun_window{default_stun_window};

    static constexpr auto default_gamma{2.F};
    float gamma{default_gamma};

    static constexpr auto default_restart_period{256};
    int32 restart_period{default_restart_period};
};

/**
    Project configuration.

    @var groups         Task groups
    @var agents         Agents that perform tasks
    @var tasks          All of the tasks
    @var opt_params     Optimization parameters
    @var bucket_size    Histogram bucket size
*/
struct Configuration {
    Groups groups;
    Agents agents;
    Tasks tasks;
    OptimizationParameters opt_params;
    std::optional<std::chrono::seconds> bucket_size;
};

/**
    Checks if any of the agents are "universal".

    A "universal" agent is an agent that can perform any task.

    @param config Configuration

    @return True if there is at least 1 universal agent.
*/
[[nodiscard]] bool
has_universal_agents(const Configuration& config) noexcept;
} // namespace angonoka
