#pragma once

#include "predict.h"

namespace angonoka{
[[nodiscard]] Histogram histogram(
    const Configuration& config,
    const OptimizedSchedule& schedule);

/**
    Find various percentiles of the histogram.

    Find the makespan for 25, 50, 75, 95 and 99 percent
    confidence intervals.

    @param histogram Makespan histogram

    @return A struct of percentile values.
*/
HistogramStats stats(const Histogram& histogram);
} // namespace angonoka
