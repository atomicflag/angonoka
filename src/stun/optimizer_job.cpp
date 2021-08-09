#include "optimizer_job.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"

namespace angonoka::stun {
OptimizerJob::OptimizerJob(
    const ScheduleParams& params,
    RandomUtils& random_utils,
    BatchSize batch_size)
    : batch_size{static_cast<std::int_fast32_t>(batch_size)}
    , mutator{params, random_utils}
    , makespan{params}
    , temperature{Beta{initial_beta}, BetaScale{beta_scale}, StunWindow{stun_window}, RestartPeriod{restart_period}}
    , stun{
          {.mutator{&mutator},
           .random{&random_utils},
           .makespan{&makespan},
           .temp{&temperature},
           .gamma{gamma}},
          initial_schedule(params)}
{
    Expects(static_cast<std::int_fast32_t>(batch_size) > 0);
}

OptimizerJob::OptimizerJob(
    const Options& options,
    BatchSize batch_size)
    : OptimizerJob{*options.params, *options.random, batch_size}
{
}

void OptimizerJob::update() noexcept
{
    Expects(batch_size > 0);

    for (int32 i{0}; i < batch_size; ++i) stun.update();
}

[[nodiscard]] Schedule OptimizerJob::schedule() const
{
    return stun.schedule();
}

[[nodiscard]] float OptimizerJob::normalized_makespan() const
{
    return stun.normalized_makespan();
}

void OptimizerJob::reset()
{
    Expects(batch_size > 0);

    stun.reset(initial_schedule(*mutator.options().params));
    temperature
        = {Beta{initial_beta},
           BetaScale{beta_scale},
           StunWindow{stun_window},
           RestartPeriod{restart_period}};
}

OptimizerJob::OptimizerJob(const OptimizerJob& other)
    : batch_size{other.batch_size}
    , mutator{other.mutator}
    , makespan{other.makespan}
    , temperature{other.temperature}
    , stun{other.stun}
{
    stun.options(
        {.mutator{&mutator},
         .random{mutator.options().random},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
}

OptimizerJob::OptimizerJob(OptimizerJob&& other) noexcept
    : batch_size{other.batch_size}
    , mutator{std::move(other.mutator)}
    , makespan{std::move(other.makespan)}
    , temperature{std::move(other.temperature)}
    , stun{std::move(other.stun)}
{
    stun.options(
        {.mutator{&mutator},
         .random{mutator.options().random},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
}

OptimizerJob& OptimizerJob::operator=(const OptimizerJob& other)
{
    if (&other == this) return *this;
    *this = OptimizerJob{other};
    return *this;
}

OptimizerJob& OptimizerJob::operator=(OptimizerJob&& other) noexcept
{
    if (&other == this) return *this;
    batch_size = other.batch_size;
    mutator = std::move(other.mutator);
    makespan = std::move(other.makespan);
    temperature = std::move(other.temperature);
    stun = std::move(other.stun);
    stun.options(
        {.mutator{&mutator},
         .random{mutator.options().random},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
    return *this;
}

void OptimizerJob::options(const Options& options)
{
    mutator.options(
        {.params{options.params}, .random{options.random}});
    stun.options(
        {.mutator{&mutator},
         .random{options.random},
         .makespan{&makespan},
         .temp{&temperature},
         .gamma{gamma}});
}

auto OptimizerJob::options() const -> Options
{
    auto [p, r] = mutator.options();
    return {p, r};
}

OptimizerJob::~OptimizerJob() noexcept = default;
} // namespace angonoka::stun
