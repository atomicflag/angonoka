#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun_dag {
using ranges::span;

/**
    General, read-only information about the schedule.

    @var agent_performance      Agent's speed multipliers
    @var task_duration          Task durations in seconds
    @var available_agents_data  Cache-friendly buffer
    @var available_agents       Which agents can perform each task
    @var dependencies_data      Cache-friendly buffer
    @var dependencies           Task's dependent sub-tasks
*/
struct ScheduleInfo {
    std::vector<float> agent_performance;
    std::vector<float> task_duration;

    std::vector<int16> available_agents_data;
    std::vector<span<int16>> available_agents;

    std::vector<int16> dependencies_data;
    std::vector<span<int16>> dependencies;
};

// TODO: Add VectorOfSpans

// class VectorOfSpans {
//     public:
//         VectorOfSpans(const VectorOfSpans& other);
//         VectorOfSpans& operator=(const VectorOfSpans& other);
//         VectorOfSpans(VectorOfSpans&& other) noexcept;
//         VectorOfSpans& operator=(VectorOfSpans&& other) noexcept;
//         ~VectorOfSpans() noexcept;
//
//         template<typename T>
//         decltype(auto) operator[](T&& key) const {
//             Expects(!spans.empty());
//             return spans[std::forward<T>(key)];
//         }
//     private:
//     std::vector<int16> data;
//     std::vector<span<int16>> spans;
// };

} // namespace angonoka::stun_dag
