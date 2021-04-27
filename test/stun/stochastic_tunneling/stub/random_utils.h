#pragma once

#include "stun/common.h"

namespace angonoka::stun {
struct RandomUtils {
    virtual float uniform_01() noexcept = 0;
    virtual int16 uniform_int(std::int16_t max) noexcept = 0;
    virtual ~RandomUtils() noexcept = default;
};
} // namespace angonoka::stun
