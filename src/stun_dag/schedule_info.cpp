#include "schedule_info.h"

namespace angonoka::stun_dag {
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
    if (other.spans.empty()) return;
    data = other.data;
    spans = other.spans;
    auto* const front_ptr = other.spans.front().data();
    for (auto& s : spans) {
        if (s.empty()) continue;
        const auto d = std::distance(front_ptr, s.data());
        s = {std::next(data.data(), d), s.size()};
    }
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
} // namespace angonoka::stun_dag
