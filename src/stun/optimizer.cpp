#include "optimizer.h"
#include "config.h"
#include <range/v3/algorithm/min.hpp>
#ifdef ANGONOKA_OPENMP
#include <omp.h>
#endif // ANGONOKA_OPENMP

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
        Expects(self.idle_iters >= 0);

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

    /**
        Convenience function to get the best job.

        @return The best optimization job.
    */
    static auto& best_job(auto& self) noexcept
    {
        Expects(!self.jobs.empty());

        return self.jobs.front().job;
    }

#ifdef ANGONOKA_OPENMP
    /**
        Replace all optimizer jobs with the best job so far.

        After each round of updates, find the best job and
        replace all other jobs with the best one. All jobs
        will continue the optimization process independently
        from the same state. Each job has it's own PRNG
        so they won't optimize exactly the same way.
    */
    static void replicate_best_job(Optimizer& self) noexcept
    {
        Expects(!self.jobs.empty());

        const auto& target_job
            = ranges::min(self.jobs, ranges::less{}, [](auto& v) {
                  return v.job.normalized_makespan();
              });
        const auto best_makespan
            = target_job.job.normalized_makespan();
        const auto params = best_job(self).params().params;
        for (auto& j : self.jobs) {
            if (j.job.normalized_makespan() == best_makespan)
                continue;
            j.job = target_job.job;
            j.job.params(
                {.params{params}, .random{&j.random_utils}});
        }
    }
#endif // ANGONOKA_OPENMP
};

Optimizer::Job::Job(const Options& options)
    : job
{
    {
        .params{options.params}, .random{&random_utils},
            .batch_size{options.batch_size},
            .beta_scale{options.beta_scale},
            .stun_window{options.stun_window}, .gamma{options.gamma},
            .restart_period
        {
            options.restart_period
        }
    }
}
{
}

Optimizer::Optimizer(const Options& options)
    : batch_size{options.batch_size}
    , max_idle_iters{options.max_idle_iters}
{
    Expects(static_cast<std::int_fast32_t>(batch_size) > 0);
    Expects(static_cast<std::int_fast32_t>(max_idle_iters) > 0);

#ifdef ANGONOKA_OPENMP
    const auto max_threads = omp_get_max_threads();
#else // ANGONOKA_OPENMP
    constexpr auto max_threads = 1;
#endif // ANGONOKA_OPENMP
    jobs.reserve(static_cast<gsl::index>(max_threads));
    for (int i{0}; i < max_threads; ++i) jobs.emplace_back(options);

    Ensures(!jobs.empty());
}

void Optimizer::update() noexcept
{
    Expects(batch_size > 0);
    Expects(!jobs.empty());

#ifdef ANGONOKA_OPENMP
#pragma omp parallel for default(none)
    for (auto& j : jobs) j.job.update();
    Impl::replicate_best_job(*this);
#else // ANGONOKA_OPENMP
    jobs.front().job.update();
#endif // ANGONOKA_OPENMP
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

    return Impl::best_job(*this).schedule();
}

[[nodiscard]] float Optimizer::normalized_makespan() const
{
    Expects(!jobs.empty());

    return Impl::best_job(*this).normalized_makespan();
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
        const auto p = j.job.params();
        j.job.params(
            {.params{&params}, .random{p.random}});
    }
}

const ScheduleParams& Optimizer::params() const
{
    Expects(!jobs.empty());

    return *Impl::best_job(*this).params().params;
}
} // namespace angonoka::stun

#pragma clang diagnostic pop

#undef TO_FLOAT
