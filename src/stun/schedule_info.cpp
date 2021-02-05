#include "schedule_info.h"

namespace angonoka::stun {
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

VectorOfSpans::VectorOfSpans(
    std::vector<int16>&& data,
    const std::vector<int16>& sizes) noexcept
    : data{std::move(data)}
{
    auto* head = this->data.data();
    for (auto&& size : sizes) {
        spans.emplace_back(
            std::exchange(head, std::next(head, size)),
            size);
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
} // namespace angonoka::stun
