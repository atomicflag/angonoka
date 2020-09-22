#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <range/v3/view/span.hpp>

// TODO: Tests, documentation, Expects

namespace angonoka::stun {
using ranges::span;
class TaskAgents {
public:
    TaskAgents() = default;
    TaskAgents(span<span<const int16>> data);

    decltype(auto) operator[](index i) const noexcept
    {
        return task_agents[i];
    }

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    std::unique_ptr<int16[]> int_data;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    std::unique_ptr<span<const int16>[]> spans;
    span<span<const int16>> task_agents;
};
} // namespace angonoka::stun
