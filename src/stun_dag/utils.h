#pragma once

#include "common.h"

namespace angonoka::stun_dag {
struct ScheduleInfo;
class RandomUtils;
/**
    TODO: Doc, implement
*/
void mutate(
    ScheduleInfo& info,
    RandomUtils& random,
    MutState state) noexcept;
} // namespace angonoka::stun_dag
