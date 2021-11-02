#pragma once

#include "stun/schedule.h"

namespace angonoka::stun {
struct Makespan {
    virtual float operator()(Schedule schedule) noexcept = 0;
    virtual ~Makespan() noexcept = default;
};

struct Mutator {
    virtual void operator()(MutSchedule schedule) const noexcept = 0;
    virtual ~Mutator() noexcept = default;
};
} // namespace angonoka::stun
