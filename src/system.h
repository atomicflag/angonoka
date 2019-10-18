#pragma once

#include <boost/container/flat_set.hpp>
#include <boost/container/small_vector.hpp>
#include <optional>
#include <random>
#include <string>

namespace angonoka {
template <typename T, auto N>
using Vector = boost::container::small_vector<T, N>;
template <typename T, auto N>
using Set = boost::container::flat_set<T, std::less<T>, Vector<T, N>>;
using GroupIds = Set<int, 5>; // NOLINT
using Normal = std::normal_distribution<float>;

/**
	Agent that performs Tasks.

	Agent::group_ids refer to Group indices in the System::groups. The
	lower the perf parameter is the slower an agent will perform any
	given task. An agent can only perform tasks belonging to groups in
	group_ids.

	@var name		Agent's name
	@var group_ids	Set of Group ids
	@var perf		Agent's performance distribution
*/
// NOLINTNEXTLINE(bugprone-exception-escape)
struct Agent {
	std::string name;
	GroupIds group_ids;
	Normal perf;
};

/**
	Task performed by an Agent.

	If a Task has a Group, it will be processed by Agents that can
	perform Tasks from that group. Otherwise it will be processed by
	any Agent.

	@var name		Task's name
	@var group_id	Task's Group id, if any
*/
struct Task {
	std::string name;
	std::optional<int> group_id;
};

using Groups = Vector<std::string, 5>; // NOLINT
using Agents = Vector<Agent, 5>; // NOLINT
using Tasks = Vector<Task, 7>; // NOLINT

/**
	System that represents Tasks and Agents.

	@var groups		Task groups
	@var agents		Agents that perform tasks
	@var tasks		All of the tasks
*/
struct System {
	Groups groups;
	Agents agents;
	Tasks tasks;
};
} // namespace angonoka
