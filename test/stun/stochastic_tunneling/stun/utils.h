#pragma once

#include "stun/common.h"

namespace angonoka::stun {
struct Makespan {
    virtual float operator()(State state) noexcept = 0;
    virtual ~Makespan() noexcept = default;
};

struct Mutator {
    virtual void operator()(MutState state) const noexcept = 0;
    virtual ~Mutator() noexcept = default;
};
} // namespace angonoka::stun
