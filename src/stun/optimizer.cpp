#include "optimizer.h"
#include <omp.h>
#include <range/v3/algorithm/min_element.hpp>

#ifndef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(x) static_cast<float>(base_value(x))
#else // NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TO_FLOAT(x) static_cast<float>(x)
#endif // NDEBUG

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"

namespace angonoka::stun {
/**
    Implementation details.
*/
struct Optimizer::Impl {
    /**
        Current progress w.r.t the maximum number of idle iterations.

        This is *not* the overall progress of the optimization
        process, this is an intra-epoch progress.

        @return Progress between 0.0 and 1.0
    */
    static float progress(Optimizer& self) noexcept
    {
        Expects(self.max_idle_iters > 0);

        return TO_FLOAT(self.idle_iters)
            / TO_FLOAT(self.max_idle_iters);
    }

    /**
        Interpolate the progress between the last epoch and the next.

        Assuming the progress is exponential, synthesize the current
        progress value based on the progress made so far.
    */
    static void interpolate_progress(Optimizer& self) noexcept
    {
        Expects(self.epochs >= 0);

        const auto p = progress(self);
        const auto next_expected
            = self.exp_curve.at(TO_FLOAT(self.epochs + 1));
        const auto next_epoch
            = TO_FLOAT(self.epochs) + p / next_expected;
        self.last_progress
            = std::min(self.exp_curve.at(next_epoch), 1.F);
    }

    /**
        Advance to the next epoch and fit the exponential curve.

        Assuming the progress is exponential, try and find the
        expected progress by fitting an exponential model.
    */
    static void estimate_progress(Optimizer& self) noexcept
    {
        Expects(self.epochs >= 0);
        Expects(!self.jobs.empty());

        const auto p = progress(self);
        self.last_makespan = self.normalized_makespan();
        self.epochs += 1;
        self.last_progress
            = std::min(self.exp_curve(TO_FLOAT(self.epochs), p), 1.F);
        self.idle_iters = 0;

        Ensures(self.epochs > 0);
        Ensures(self.last_progress >= 0.F);
    }

    // TODO: doc, test, expects
    static void replicate_best_job(Optimizer& self) noexcept
    {
        const auto target_job = ranges::min_element(
            self.jobs,
            ranges::less{},
            [](auto& v) { return v.job.normalized_makespan(); });
        const auto params = self.jobs.front().job.options().params;
        for (auto j{self.jobs.begin()}; j < self.jobs.end(); ++j) {
            if (j == target_job) continue;
            j->job = target_job->job;
            j->job.options(
                {.params{params}, .random{&j->random_utils}});
        }
    }
};

Optimizer::Job::Job(
    const ScheduleParams& params,
    BatchSize batch_size)
    : job{params, random_utils, batch_size}
{
}

Optimizer::Optimizer(
    const ScheduleParams& params,
    BatchSize batch_size,
    MaxIdleIters max_idle_iters)
    : batch_size{static_cast<std::int_fast32_t>(batch_size)}
    , max_idle_iters{static_cast<std::int_fast32_t>(max_idle_iters)}
{
    Expects(static_cast<std::int_fast32_t>(batch_size) > 0);
    Expects(static_cast<std::int_fast32_t>(max_idle_iters) > 0);

    const auto max_threads = omp_get_max_threads();
    jobs.reserve(static_cast<gsl::index>(max_threads));
    for (int i{0}; i < max_threads; ++i)
        jobs.emplace_back(params, batch_size);

    Ensures(!jobs.empty());
}

void Optimizer::update() noexcept
{
    Expects(batch_size > 0);
    Expects(!jobs.empty());

#pragma omp parallel for default(none)
    for (auto& j : jobs) j.job.update();
    Impl::replicate_best_job(*this);
    idle_iters += batch_size;
    // Make up a progress value just so that the user
    // doesn't think that the optimizaton has halted.
    if (normalized_makespan() == last_makespan) {
        Impl::interpolate_progress(*this);
        return;
    }
    Impl::estimate_progress(*this);
}

[[nodiscard]] bool Optimizer::has_converged() const noexcept
{
    Expects(max_idle_iters > 0);

    return idle_iters >= max_idle_iters;
}

[[nodiscard]] float Optimizer::estimated_progress() const noexcept
{
    Expects(idle_iters >= 0);
    Expects(max_idle_iters > 0);

    if (has_converged()) return 1.F;
    constexpr auto warmup_epochs = 5;
    if (epochs < warmup_epochs) return 0.F;
    return last_progress;
}

[[nodiscard]] Schedule Optimizer::schedule() const
{
    Expects(!jobs.empty());

    return jobs.front().job.schedule();
}

[[nodiscard]] float Optimizer::normalized_makespan() const
{
    Expects(!jobs.empty());

    return jobs.front().job.normalized_makespan();
}

void Optimizer::reset()
{
    Expects(max_idle_iters > 0);
    Expects(batch_size > 0);
    Expects(!jobs.empty());

    idle_iters = 0;
    epochs = 0;
    last_progress = 0.F;
    last_makespan = 0.F;
    exp_curve.reset();

    for (auto& j : jobs) j.job.reset();
}

void Optimizer::params(const ScheduleParams& params)
{
    Expects(!jobs.empty());

    for (auto& j : jobs) {
        j.job.options({.params{&params}, .random{&j.random_utils}});
    }
}

const ScheduleParams& Optimizer::params() const
{
    Expects(!jobs.empty());

    return *jobs.front().job.options().params;
}
} // namespace angonoka::stun

#pragma clang diagnostic pop

#undef TO_FLOAT
