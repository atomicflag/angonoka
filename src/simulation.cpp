#include "simulation.h"
#include <algorithm>
#include <array>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/extended_p_square.hpp>
#include <boost/accumulators/statistics/rolling_count.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/view/transform.hpp>

namespace {
using namespace boost::accumulators;
using PSquareAcc
    = accumulator_set<float, stats<tag::extended_p_square>>;
using RollingMeanAcc
    = accumulator_set<float, stats<tag::rolling_mean>>;
using namespace angonoka;
constexpr auto quantile_count = 5;
using Quantiles = std::array<float, quantile_count>;

/**
    Mean absolute percentage error of the histogram quantiles.

    Quick and dirty way to tell how the accuracy of the histogram
    improved since the last batch.

    We don't need sub-minute accuracy but we also don't want too
    sparse of a histogram. This function is used as a stopping
    condition for the simulation. The simulation stops when we
    measure very little further improvement, when the simulation
    converges.

    @param acc			Quantile accumulator with new samples
    @param quantiles    Last batch's quantiles

    @return Error value, smaller value means higher accuracy
*/
[[nodiscard]] float mean_absolute_pct_error(
    const PSquareAcc& acc,
    Quantiles& quantiles) noexcept
{
    Expects(!quantiles.empty());

    const auto p_square = extended_p_square(acc);
    auto error = 0.F;
    for (gsl::index i{0}; i < quantiles.size(); ++i) {
        const auto a = quantiles[i];
        const auto b = p_square[gsl::narrow<long>(i)];
        error += std::abs((a - b) / a);
    }
    // range/v3 version is too strict w.r.t range requirements
    std::copy(p_square.begin(), p_square.end(), quantiles.begin());

    Ensures(error >= 0.F);

    return error / gsl::narrow<float>(quantiles.size());
}

/**
    Convert Boost's safe numerics to seconds.

    @param value Safe numeric value

    @return Value in seconds
*/
[[nodiscard]] std::chrono::seconds to_seconds(int32 value) noexcept
{
    Expects(value >= 0);

    return std::chrono::seconds{base_value(value)};
}
} // namespace

namespace angonoka::detail {
using angonoka::stun::int16;
using index_type = ranges::span<float>::index_type;

/**
    Implementation details.
*/
struct Simulation::Impl {
    /**
        Reset the buffer and assign views.

        Resizes the buffer to the new size and
        reassignes the spans to subsections of the
        new buffer.
    */
    static void assign_buffers(Simulation& self)
    {
        using ranges::span;
        const auto& agents = self.config->agents;
        const auto& tasks = self.config->tasks;

        Expects(!agents.empty());
        Expects(!tasks.empty());

        self.buffer.resize(agents.size() * 2 + tasks.size() * 2);
        const auto agent_count = std::ssize(agents);
        const auto task_count = std::ssize(tasks);
        auto* head = self.buffer.data();
        const auto next_span = [&](auto count) {
            return span{
                std::exchange(head, std::next(head, count)),
                count};
        };
        self.agent_performance = next_span(agent_count);
        self.task_duration = next_span(task_count);
        self.agent_work_end = next_span(agent_count);
        self.task_done = next_span(task_count);

        Ensures(
            self.buffer.size()
            == agents.size() * 2 + tasks.size() * 2);
        Ensures(self.agent_performance.data() == self.buffer.data());
        Ensures(self.agent_performance.size() == std::ssize(agents));
        Ensures(self.task_duration.size() == std::ssize(tasks));
        Ensures(self.agent_work_end.size() == std::ssize(agents));
        Ensures(self.task_done.size() == std::ssize(tasks));
    }

    /**
        Picks random agent performances.

        Agent performances follow the gaussian distribution.
    */
    static void random_agent_performances(Simulation& self)
    {
        Expects(!self.config->agents.empty());
        Expects(
            std::ssize(self.config->agents)
            == self.agent_performance.size());

        for (index_type i = 0; auto&& agent : self.config->agents) {
            const auto min = agent.performance.min;
            const auto max = agent.performance.max;
            if (min == max) {
                self.agent_performance[i] = min;
                ++i;
                continue;
            }
            float perf = self.random->normal(min, max);
            while (perf <= 0.F) perf = self.random->normal(min, max);
            self.agent_performance[i] = perf;
            ++i;
        }
    }

    /**
        Picks random task durations.

        Task durations follow the gaussian distribution.
    */
    static void random_task_durations(Simulation& self)
    {
        Expects(!self.config->tasks.empty());
        Expects(
            std::ssize(self.config->tasks)
            == self.task_duration.size());

        for (index_type i = 0; auto&& task : self.config->tasks) {
            const auto min
                = static_cast<float>(task.duration.min.count());
            const auto max
                = static_cast<float>(task.duration.max.count());
            if (min == max) {
                self.task_duration[i] = min;
                ++i;
                continue;
            }
            float duration = self.random->normal(min, max);
            while (duration <= 0.F)
                duration = self.random->normal(min, max);
            self.task_duration[i] = duration;
            ++i;
        }
    }

    /**
        How long it will take for a given agent to complete a given
        task.

        Factors in agent's performace.

        @param task_id Task's index
        @param agent_id Agent's index

        @return Time in seconds
    */
    [[nodiscard]] static float task_duration(
        const Simulation& self,
        int16 task_id,
        int16 agent_id)
    {
        Expects(agent_id >= 0);
        Expects(agent_id < std::ssize(self.config->agents));
        Expects(task_id >= 0);
        Expects(task_id < std::ssize(self.config->tasks));

        const auto performance = self.agent_performance[agent_id];
        const auto task_duration = self.task_duration[task_id];

        Expects(performance > 0.F);
        Expects(task_duration > 0.F);

        return static_cast<float>(task_duration) / performance;
    }

    /**
        The time when the last dependency of a given task will be
        completed.

        @param task_id Task's index

        @return Time in seconds
    */
    [[nodiscard]] static float
    dependencies_done(const Simulation& self, const TaskIndices& deps)
    {
        Expects(!self.task_done.empty());

        using ranges::max;
        using ranges::views::transform;
        if (deps.empty()) return 0.F;
        return max(deps | transform([&](auto dep_id) {
                       return self.task_done[dep_id];
                   }));
    }
};

Simulation::Simulation(const Params& params)
    : config{params.config}
    , random{params.random}
{
    Expects(!config->agents.empty());
    Expects(!config->tasks.empty());

    Impl::assign_buffers(*this);

    Ensures(
        buffer.size()
        == config->agents.size() * 2 + config->tasks.size() * 2);
    Ensures(agent_performance.data() == buffer.data());
    Ensures(agent_performance.size() == std::ssize(config->agents));
    Ensures(task_duration.size() == std::ssize(config->tasks));
    Ensures(agent_work_end.size() == std::ssize(config->agents));
    Ensures(task_done.size() == std::ssize(config->tasks));
}

[[nodiscard]] auto Simulation::params() const -> Params
{
    return {.config{config}, .random{random}};
}

void Simulation::params(const Params& params)
{
    Expects(!config->agents.empty());
    Expects(!config->tasks.empty());

    config = params.config;
    random = params.random;
    Impl::assign_buffers(*this);

    Ensures(
        buffer.size()
        == config->agents.size() * 2 + config->tasks.size() * 2);
    Ensures(agent_performance.data() == buffer.data());
    Ensures(agent_performance.size() == std::ssize(config->agents));
    Ensures(task_duration.size() == std::ssize(config->tasks));
    Ensures(agent_work_end.size() == std::ssize(config->agents));
    Ensures(task_done.size() == std::ssize(config->tasks));
}

[[nodiscard]] std::chrono::seconds Simulation::operator()(
    ranges::span<const stun::ScheduleItem> schedule) noexcept
{
    Expects(!buffer.empty());
    Expects(!agent_performance.empty());
    Expects(!task_duration.empty());
    Expects(!agent_work_end.empty());
    Expects(!task_done.empty());
    Expects(std::ssize(schedule) == task_done.size());
    Expects(agent_performance.data() == buffer.data());

    ranges::fill(buffer, 0.F);
    Impl::random_agent_performances(*this);
    Impl::random_task_durations(*this);
    for (auto [task_id, agent_id] : schedule) {
        const auto& task
            = config->tasks[static_cast<gsl::index>(task_id)];
        const auto deps_done
            = Impl::dependencies_done(*this, task.dependencies);
        const auto done
            = std::max(deps_done, agent_work_end[agent_id])
            + Impl::task_duration(*this, task_id, agent_id);
        agent_work_end[agent_id] = task_done[task_id] = done;
    }
    using rep = std::chrono::seconds::rep;
    return std::chrono::seconds{
        static_cast<rep>(ranges::max(agent_work_end))};
}

Simulation::Simulation(const Simulation& other)
    : config{other.config}
    , random{other.random}
    , buffer{other.buffer}
{
    Expects(!other.buffer.empty());

    Impl::assign_buffers(*this);

    Ensures(!buffer.empty());
    Ensures(buffer.size() == other.buffer.size());
}

Simulation::Simulation(Simulation&& other) noexcept = default;

Simulation& Simulation::operator=(const Simulation& other)
{
    Expects(!other.buffer.empty());

    config = other.config;
    random = other.random;
    buffer = other.buffer;

    Impl::assign_buffers(*this);

    Ensures(!buffer.empty());
    Ensures(buffer.size() == other.buffer.size());

    return *this;
}

Simulation&
Simulation::operator=(Simulation&& other) noexcept = default;

Simulation::~Simulation() noexcept = default;

[[nodiscard]] int32 granularity(std::chrono::seconds makespan)
{
    using namespace std::chrono_literals;
    using std::chrono::days;
    using std::chrono::duration_cast;
    using std::chrono::seconds;

    Expects(makespan >= 1s);

    constexpr auto minutes_threshold = 5h;
    if (makespan < minutes_threshold)
        return duration_cast<seconds>(1min).count();
    constexpr auto hours_threshold = days{13};
    if (makespan < hours_threshold)
        return duration_cast<seconds>(1h).count();
    return duration_cast<seconds>(days{1}).count();
}
} // namespace angonoka::detail

namespace {
using namespace angonoka;
using angonoka::detail::granularity;
using angonoka::detail::Histogram;
using angonoka::detail::Simulation;

/**
    Pick a bin size for the histogram.

    @param config   Project configuration
    @param schedule Optimized schedule

    @return Histogram granularity in seconds.
*/
[[nodiscard]] int32 parse_granularity(
    const Project& config,
    const OptimizedSchedule& schedule)
{
    using namespace std::chrono_literals;

    Expects(schedule.makespan >= 1s);

    if (config.bin_size) return config.bin_size->count();
    return granularity(schedule.makespan);
}

/**
    Function object for making a histogram of simulation results.
*/
struct HistogramOp {
    gsl::not_null<const Project*> config;
    gsl::not_null<const OptimizedSchedule*> schedule;
    stun::RandomUtils random{};
    Simulation sim{{.config{config}, .random{&random}}};
    Histogram hist{parse_granularity(*config, *schedule)};
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    static constexpr Quantiles probs
        = {0.25F, 0.50F, 0.75F, 0.95F, 0.99F};
    PSquareAcc acc{tag::extended_p_square::probabilities = probs};
    RollingMeanAcc rolling_error{
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
        tag::rolling_window::window_size = 15};
    Quantiles quantiles{};
    static constexpr auto batch_size = 10'000;

    /**
        Run batch_size simulations and log results.

        Logs P-square quantiles and accumulates the histogram.
    */
    void run_simulation_batch()
    {
        Expects(!schedule->schedule.empty());

        for (int i = 0; i < batch_size; ++i) {
            const auto makespan = sim(schedule->schedule).count();
            acc(gsl::narrow_cast<float>(makespan));
            hist(makespan);
        }
    }

    /**
        Run the simulations and build a histogram.

        First runs one batch to get the baseline for the makespan
        quantiles and then runs simulations until batch-to-batch
        quantile estimation accuracy improvement falls below the
        threshold.
    */
    [[nodiscard]] Histogram operator()()
    {
        Expects(quantiles.size() > 1);

        ranges::fill(quantiles, 1.F);
        run_simulation_batch();
        rolling_error(mean_absolute_pct_error(acc, quantiles));
        constexpr auto error_threshold = 0.01F;
        while (rolling_mean(rolling_error) > error_threshold) {
            run_simulation_batch();
            rolling_error(mean_absolute_pct_error(acc, quantiles));
        }

        Ensures(hist.size() > 0);
        return hist;
    }
};
} // namespace

namespace angonoka {
[[nodiscard]] detail::Histogram
histogram(const Project& config, const OptimizedSchedule& schedule)
{
    Expects(!config.tasks.empty());
    Expects(!config.agents.empty());
    Expects(!schedule.schedule.empty());
    return HistogramOp{&config, &schedule}();
}

HistogramStats stats(const detail::Histogram& histogram)
{
    using ranges::back;

    Expects(histogram.size() > 0);

    const float total = ranges::accumulate(histogram, 0.F);
    HistogramStats stats;
    float count = 0.F;
    int bin_index = 0;

    // Accumulates histogram bins until the threshold is reached
    const auto accumulate_until = [&](auto threshold) {
        Expects(threshold >= 0.F);

        for (; bin_index < std::ssize(histogram); ++bin_index) {
            const auto bin = histogram[bin_index];
            count += static_cast<float>(bin);
            if (count >= threshold) {
                ++bin_index;
                return to_seconds(bin.middle);
            }
        }
        return to_seconds(back(histogram).middle);
    };

    stats.p25 = accumulate_until(total * 0.25F); // NOLINT
    stats.p50 = accumulate_until(total * 0.50F); // NOLINT
    stats.p75 = accumulate_until(total * 0.75F); // NOLINT
    stats.p95 = accumulate_until(total * 0.95F); // NOLINT
    stats.p99 = accumulate_until(total * 0.99F); // NOLINT

    return stats;
}
} // namespace angonoka
