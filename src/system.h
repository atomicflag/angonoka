#pragma once

#include <optional>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

namespace angonoka {
using GroupIds = std::unordered_set<int>;
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

using Groups = std::vector<std::string>;
using Agents = std::vector<Agent>;
using Tasks = std::vector<Task>;

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
