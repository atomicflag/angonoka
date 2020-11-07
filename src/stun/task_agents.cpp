#include "task_agents.h"

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/zip.hpp>

namespace angonoka::stun {
/**
    Sums up lengths of all sub-arrays.

    @param data Array of arrays

    @return Sum of sizes of all sub-arrays.
*/
static gsl::index total_size(span<const span<const int16>> data)
{
    Expects(!data.empty());

    const auto result = ranges::accumulate(
        data,
        gsl::index{},
        [](auto acc, auto&& i) {
            return acc + static_cast<gsl::index>(i.size());
        });

    Ensures(result > 0);

    return result;
}

TaskAgents::TaskAgents(span<const span<const int16>> data)
    : int_data(total_size(data))
    , task_agents(static_cast<gsl::index>(data.size()))
{
    Expects(!data.empty());

    int16* int_data_ptr = int_data.data();
    using ranges::views::zip;
    for (auto&& [v, a] : zip(data, task_agents)) {
        a = {int_data_ptr, static_cast<index>(v.size())};
        int_data_ptr = ranges::copy(v, int_data_ptr).out;
    }

    Ensures(!int_data.empty());
    Ensures(!task_agents.empty());
    Ensures(
        task_agents.size() == static_cast<gsl::index>(data.size()));
}

TaskAgents::TaskAgents(const TaskAgents& other)
    : TaskAgents{other.task_agents}
{
}

TaskAgents& TaskAgents::operator=(const TaskAgents& other)
{
    *this = TaskAgents{other.task_agents};
    return *this;
}
} // namespace angonoka::stun
