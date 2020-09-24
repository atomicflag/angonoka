#include "task_agents.h"

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/numeric/accumulate.hpp>

namespace angonoka::stun {
/**
    Sums up lengths of all sub-arrays.

    @param data Array of arrays

    @return Sum of sizes of all sub-arrays.
*/
static gsl::index total_size(span<span<const int16>> data)
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

TaskAgents::TaskAgents(span<span<const int16>> data)
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    : int_data{std::make_unique<int16[]>(total_size(data))}
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    , spans{std::make_unique<span<const int16>[]>(
          static_cast<gsl::index>(data.size()))}
    , task_agents{spans.get(), static_cast<long>(data.size())}
{
    Expects(!data.empty());

    int16* int_data_ptr = int_data.get();
    span<const int16>* spans_ptr = spans.get();
    for (auto&& v : data) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        *spans_ptr++ = {int_data_ptr, static_cast<long>(v.size())};
        int_data_ptr = ranges::copy(v, int_data_ptr).out;
    }

    Ensures(spans);
    Ensures(int_data);
    Ensures(!task_agents.empty());
}
} // namespace angonoka::stun
