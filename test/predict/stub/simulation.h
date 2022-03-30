#pragma once

#include "predict.h"

namespace angonoka {
[[nodiscard]] Histogram histogram(
    const Configuration& config,
    const OptimizedSchedule& schedule);
HistogramStats stats(const Histogram& histogram);
} // namespace angonoka
