#pragma once

#include "configuration.h"
#include "predict.h"
#include "stun/random_utils.h"
#include <chrono>
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include <vector>

namespace angonoka {
namespace detail {
    // TODO: doc, test, expects
    class Simulation {
    public:
        // TODO: doc, test, expects
        Simulation(
            const Configuration& config,
            const OptimizedSchedule& schedule,
            stun::RandomUtils& random);

        // TODO: doc, test, expects
        [[nodiscard]] std::chrono::seconds operator()() noexcept;

    private:
        struct Impl;
        gsl::not_null<const Configuration*> config;
        gsl::not_null<const OptimizedSchedule*> schedule;
        gsl::not_null<stun::RandomUtils*> random;
        std::vector<float> buffer;
        ranges::span<float> agent_performance;
        ranges::span<float> task_duration;
        ranges::span<float> agent_work_end;
        ranges::span<float> task_done;
    };
} // namespace detail
// boost::histogram histogram(
//   const Configuration& config,
//   const OptimizedSchedule& schedule);
} // namespace angonoka
