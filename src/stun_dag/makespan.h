#pragma once

#include "common.h"
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::stun_dag {
using ranges::span;
struct ScheduleInfo;

enum class TasksCount : int {};
enum class AgentsCount : int {};

// TODO: test
class Makespan {
public:
    Makespan(
        gsl::not_null<ScheduleInfo*> info,
        TasksCount tasks_count,
        AgentsCount agents_count);
    Makespan(const Makespan& other);
    Makespan& operator=(const Makespan& other) noexcept;
    Makespan(Makespan&& other) noexcept;
    Makespan& operator=(Makespan&& other) noexcept;
    ~Makespan() noexcept;

    float operator()(State state) noexcept;

private:
    gsl::not_null<ScheduleInfo*> info;
    std::vector<float> sum_buffer;
    span<float> task_done;
    span<float> work_done;

    [[nodiscard]] float dependency_done(int16 task_id) const noexcept;

    [[nodiscard]] float
    task_duration(int16 task_id, int16 agent_id) const noexcept;
};
} // namespace angonoka::stun_dag