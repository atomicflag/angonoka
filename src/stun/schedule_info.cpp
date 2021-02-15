#include "schedule_info.h"
#include <boost/container/flat_set.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/iota.hpp>

namespace angonoka::stun {
using boost::container::flat_set;

VectorOfSpans::VectorOfSpans(
    std::vector<int16>&& data,
    std::vector<span<int16>>&& spans) noexcept
    : data{std::move(data)}
    , spans{std::move(spans)}
{
}

void VectorOfSpans::clear() noexcept
{
    data.clear();
    spans.clear();
}

VectorOfSpans::VectorOfSpans() noexcept = default;
VectorOfSpans::VectorOfSpans(const VectorOfSpans& other)
{
    if (other.empty()) return;
    data = other.data;
    spans = other.spans;
    auto* const front_ptr = other.spans.front().data();
    for (auto& s : spans) {
        if (s.empty()) continue;
        const auto d = std::distance(front_ptr, s.data());
        s = {std::next(data.data(), d), s.size()};
    }

    Ensures(other.size() == size());
}

VectorOfSpans::VectorOfSpans(
    std::vector<int16>&& data,
    span<const int16> sizes) noexcept
    : data{std::move(data)}
{
    if (sizes.empty()) return;
    auto* head = this->data.data();
    for (auto&& size : sizes) {
        spans.emplace_back(
            std::exchange(head, std::next(head, size)),
            size);
    }

    Ensures(std::ssize(spans) == sizes.size());
}

[[nodiscard]] std::size_t VectorOfSpans::size() const noexcept
{
    return spans.size();
}

[[nodiscard]] bool VectorOfSpans::empty() const noexcept
{
    return spans.empty();
}

VectorOfSpans& VectorOfSpans::operator=(const VectorOfSpans& other)
{
    *this = VectorOfSpans{other};
    return *this;
}

VectorOfSpans::VectorOfSpans(
    VectorOfSpans&& other) noexcept = default;
VectorOfSpans&
VectorOfSpans::operator=(VectorOfSpans&& other) noexcept = default;
VectorOfSpans::~VectorOfSpans() noexcept = default;

// TODO: doc, test
void push_task(
    std::vector<StateItem>& state,
    flat_set<int16>& tasks,
    int16 task_index,
    const ScheduleInfo& info)
{
    Expects(!tasks.empty());
    Expects(tasks.size() + state.size() == info.task_duration.size());

    if (!tasks.contains(task_index)) return;
    const auto idx = static_cast<std::size_t>(task_index);
    for (auto&& dep_index : info.dependencies[idx])
        push_task(state, tasks, dep_index, info);
    state.emplace_back(StateItem{
        .task_id = task_index,
        .agent_id = info.available_agents[idx][0]});
    tasks.erase(task_index);

    Ensures(tasks.size() + state.size() == info.task_duration.size());
}

std::vector<StateItem> initial_state(const ScheduleInfo& info)
{
    using ranges::front;
    using ranges::views::iota;

    Expects(!info.task_duration.empty());

    std::vector<StateItem> state;
    state.reserve(info.task_duration.size());
    auto tasks = iota(0L, std::ssize(info.task_duration))
        | ranges::to<flat_set<int16>>();
    while (!tasks.empty())
        push_task(state, tasks, front(tasks), info);

    Ensures(state.size() == info.task_duration.size());

    return state;
}
} // namespace angonoka::stun
