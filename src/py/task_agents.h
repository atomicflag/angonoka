#pragma once

#include "common.h"
#include <range/v3/view/span.hpp>
#include <gsl/gsl-lite.hpp>
#include <memory>

namespace angonoka::stun {
    using ranges::span;
class TaskAgents {
public:
    TaskAgents() = default;
    TaskAgents(span<const int16> data);

    decltype(auto) operator[](gsl::index i) const noexcept
{
    return task_agents[i];
}

private:
    std::unique_ptr<int16[]> int_data;
    std::unique_ptr<span<const int16>[]> spans;
    span<span<const int16>> task_agents;
};
} // namespace angonoka::stun
