#pragma once

#include "predict.h"

namespace angonoka {
[[nodiscard]] detail::Histogram
histogram(const Project& config, const OptimizedSchedule& schedule);
HistogramStats stats(const detail::Histogram& histogram);
} // namespace angonoka
