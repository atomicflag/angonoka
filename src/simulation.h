#pragma once

#include "configuration.h"
#include "predict.h"
#include "stun/random_utils.h"
#include <chrono>
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka::detail {
// TODO: doc, test, expects
class Simulation {
public:
    // TODO: doc, test, expects
    struct Params {
        gsl::not_null<const Configuration*> config;
        gsl::not_null<stun::RandomUtils*> random;
    };

    // TODO: doc, test, expects
    Simulation(const Params& params);

    // TODO: doc, test, expects
    [[nodiscard]] std::chrono::seconds
    operator()(const OptimizedSchedule& schedule) noexcept;

    // TODO: doc, test, expects
    [[nodiscard]] Params params() const;

    // TODO: doc, test, expects
    void params(const Params& params);

    // TODO: rule of 5

private:
    struct Impl;
    gsl::not_null<const Configuration*> config;
    gsl::not_null<stun::RandomUtils*> random;
    std::vector<float> buffer;
    ranges::span<float> agent_performance;
    ranges::span<float> task_duration;
    ranges::span<float> agent_work_end;
    ranges::span<float> task_done;
};
} // namespace angonoka::detail

namespace angonoka {
// boost::histogram histogram(
//   const Configuration& config,
//   const OptimizedSchedule& schedule);
} // namespace angonoka
