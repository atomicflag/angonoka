#pragma once

#include "configuration.h"
#include "options.h"
#include "predict.h"
#include <nlohmann/json.hpp>

namespace angonoka::cli {
namespace detail {
    // TODO: doc, test, expects
    nlohmann::json to_json(
        const Configuration& config,
        const OptimizedSchedule& schedule);
} // namespace detail
// TODO: doc, test, expects
nlohmann::json
json_schedule(const Configuration& config, const Options& options);
} // namespace angonoka::cli
