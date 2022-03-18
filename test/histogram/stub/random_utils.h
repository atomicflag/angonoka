#pragma once

#include "stun/schedule.h"

namespace angonoka::stun {
struct RandomUtils {
    float normal(float min, float max) noexcept;
};
} // namespace angonoka::stun
