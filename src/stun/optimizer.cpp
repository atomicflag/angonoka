#include "optimizer.h"

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

        const auto p = progress(self);
        self.last_makespan = self.stun.normalized_makespan();
        self.epochs += 1;
        self.last_progress
            = std::min(self.exp_curve(TO_FLOAT(self.epochs), p), 1.F);
        self.idle_iters = 0;

        Ensures(self.epochs > 0);
        Ensures(self.last_progress >= 0.F);
    }
};

Optimizer::Optimizer(
    const ScheduleParams& params,
    BatchSize batch_size,
    MaxIdleIters max_idle_iters)
    : params{&params}
    , batch_size{static_cast<std::int_fast32_t>(batch_size)}
    , max_idle_iters{static_cast<std::int_fast32_t>(max_idle_iters)}
{
    Expects(static_cast<std::int_fast32_t>(batch_size) > 0);
    Expects(static_cast<std::int_fast32_t>(max_idle_iters) > 0);
}

void Optimizer::update() noexcept
{
    Expects(batch_size > 0);

    for (int32 i{0}; i < batch_size; ++i) stun.update();
    idle_iters += batch_size;
    // Make up a progress value just so that the user
    // doesn't think that the optimizaton has halted.
    if (stun.normalized_makespan() == last_makespan) {
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
    return stun.schedule();
}

[[nodiscard]] float Optimizer::normalized_makespan() const
{
    return stun.normalized_makespan();
}

void Optimizer::reset()
{
    Expects(max_idle_iters > 0);
    Expects(batch_size > 0);

    stun.reset(initial_schedule(*params));
    temperature
        = {Beta{initial_beta},
           BetaScale{beta_scale},
           StunWindow{stun_window},
           RestartPeriod{restart_period}};
    idle_iters = 0;
    epochs = 0;
    last_progress = 0.F;
    last_makespan = 0.F;
    exp_curve.reset();
}

Optimizer::Optimizer(const Optimizer& other)
    : params{other.params}
    , batch_size{other.batch_size}
    , max_idle_iters{other.max_idle_iters}
    , idle_iters{other.idle_iters}
    , epochs{other.epochs}
    , last_progress{other.last_progress}
    , last_makespan{other.last_makespan}
    , random_utils{other.random_utils}
    , mutator{other.mutator}
    , makespan{other.makespan}
    , temperature{other.temperature}
    , stun{other.stun}
    , exp_curve{other.exp_curve}
{
    stun.options(
        {.mutator{&mutator},
         .random{&random_utils},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
}

Optimizer::Optimizer(Optimizer&& other) noexcept
    : params{std::move(other.params)}
    , batch_size{other.batch_size}
    , max_idle_iters{other.max_idle_iters}
    , idle_iters{other.idle_iters}
    , epochs{other.epochs}
    , last_progress{other.last_progress}
    , last_makespan{other.last_makespan}
    , random_utils{other.random_utils}
    , mutator{std::move(other.mutator)}
    , makespan{std::move(other.makespan)}
    , temperature{std::move(other.temperature)}
    , stun{std::move(other.stun)}
    , exp_curve{other.exp_curve}
{
    stun.options(
        {.mutator{&mutator},
         .random{&random_utils},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
}

Optimizer& Optimizer::operator=(const Optimizer& other)
{
    if (&other == this) return *this;
    *this = Optimizer{other};
    return *this;
}

Optimizer& Optimizer::operator=(Optimizer&& other) noexcept
{
    if (&other == this) return *this;
    params = other.params;
    batch_size = other.batch_size;
    max_idle_iters = other.max_idle_iters;
    idle_iters = other.idle_iters;
    epochs = other.epochs;
    last_progress = other.last_progress;
    last_makespan = other.last_makespan;
    random_utils = other.random_utils;
    mutator = std::move(other.mutator);
    makespan = std::move(other.makespan);
    temperature = std::move(other.temperature);
    stun = std::move(other.stun);
    stun.options(
        {.mutator{&mutator},
         .random{&random_utils},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
    exp_curve = other.exp_curve;
    return *this;
}

Optimizer::~Optimizer() noexcept = default;
} // namespace angonoka::stun

#pragma clang diagnostic pop

#undef TO_FLOAT
