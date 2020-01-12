#pragma once

#include <boost/container/flat_set.hpp>
#include <boost/container/small_vector.hpp>
#include <optional>
#include <string>

namespace angonoka {
template <typename T, auto N>
using Vector = boost::container::small_vector<T, N>;
template <typename T, auto N>
using Set = boost::container::flat_set<T, std::less<T>, Vector<T, N>>;
using GroupIds = Set<int, 5>;

/**
	Agent that performs Tasks.

	Agent::group_ids refer to Group indices in the System::groups. The
	lower the perf parameter is the slower an agent will perform any
	given task. An agent can only perform tasks belonging to groups in
	group_ids.

	@var name		Agent's name
	@var group_ids	Set of Group ids
	@var perf		Performance min/max
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
	Performance perf;
};

/**
	Task performed by an Agent.

	If a Task has a Group, it will be processed by Agents that can
	perform Tasks from that group. Otherwise it will be processed by
	any Agent.

	@var name		Task's name
	@var group_id	Group id, if any
	@var dur		Duration min/max in seconds
*/
struct Task {
	std::string name;
	std::optional<int> group_id;
	struct Duration {
		int min, max;
	};
	Duration dur{-1, -1};
};

using Groups = Vector<std::string, 5>;
using Agents = Vector<Agent, 5>;
using Tasks = Vector<Task, 7>;

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
