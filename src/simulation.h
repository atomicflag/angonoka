#pragma once

#include "configuration.h"
#include "stun/random_utils.h"
#include "predict.h"
#include <chrono>

namespace angonoka {
namespace detail {
// TODO: doc, test, expects
[[nodiscard]] std::chrono::seconds run_simulation(
  const Configuration& config,
  const OptimizedSchedule& schedule,
  stun::RandomUtils& random) {
    return {};
}
}
// boost::histogram histogram(
//   const Configuration& config,
//   const OptimizedSchedule& schedule);
}
