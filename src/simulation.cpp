#include "simulation.h"
#include <boost/histogram/accumulators/mean.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/transform.hpp>

namespace {
using namespace angonoka;
/**
    Return the middle value of the histogram bin.

    @param histogram Histogram of simulation makespans
    @param bin       Index of the bin

    @return Bin middle value in seconds.
*/
std::chrono::seconds
bin_middle_value(const Histogram& histogram, int bin)
{
    using std::chrono::seconds;
    using rep = seconds::rep;

    Expects(bin >= 0);
    Expects(histogram.size() > 0);

    if (bin >= std::ssize(histogram))
        bin = gsl::narrow<int>(std::ssize(histogram) - 1);
    return seconds{
        gsl::narrow<rep>(histogram.axis().bin(bin).center())};
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

// TODO: benchmark and optimize
[[nodiscard]] std::chrono::seconds Simulation::operator()(
    ranges::span<const stun::ScheduleItem> schedule) noexcept
{
    Expects(!buffer.empty());
    Expects(!agent_performance.empty());
    Expects(!task_duration.empty());
    Expects(!agent_work_end.empty());
    Expects(!task_done.empty());
    Expects(schedule.size() == task_done.size());
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

[[nodiscard]] float granularity(std::chrono::seconds makespan)
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

namespace angonoka {
[[nodiscard]] Histogram histogram(
    const Configuration& config,
    const OptimizedSchedule& schedule)
{
    using boost::histogram::accumulators::mean;
    using detail::granularity;

    Expects(!config.tasks.empty());
    Expects(!config.agents.empty());
    Expects(!schedule.schedule.empty());

    stun::RandomUtils random;
    detail::Simulation sim{{.config{&config}, .random{&random}}};
    fmt::print(
        "DBG: granularity = {}\n",
        granularity(schedule.makespan));
    fmt::print(
        "DBG: granularity = {}\n",
        granularity(schedule.makespan));
    Histogram hist{{{1, 0.F, granularity(schedule.makespan)}}};
    mean<float> var_acc;
    const auto burn_in_count
        = std::max(1000LL, schedule.makespan.count());
    fmt::print("DBG: burn_in_count = {}\n", burn_in_count);
    fmt::print("DBG: burn_in_count = {}\n", burn_in_count);
    for (int i{0}; i < burn_in_count; ++i) {
        const auto makespan = sim(schedule.schedule).count();
        var_acc(gsl::narrow_cast<float>(makespan));
        hist(makespan);
    }
    const auto var = var_acc.variance();
    fmt::print("DBG: variance = {}\n", var);
    // (4*Z^2*var)/(W^2)
    // (4*1.96^2*var)/(60 sec ^2)
    // sample_coeff = (4*1.96^2)/(60^2)
    // TODO: make accuracy (60 sec) customizable
    const auto sample_coeff = 0.004268F;
    const std::uint64_t sample_size
        = gsl::narrow<std::uint64_t>(std::ceil(sample_coeff * var));
    fmt::print("DBG: sample_size = {}\n", sample_size);
    // TODO: this is too slow
    for (std::uint64_t i{0U}; i < sample_size; ++i)
        hist(sim(schedule.schedule).count());

    return hist;
}

HistogramStats stats(const Histogram& histogram)
{
    Expects(histogram.size() > 0);

    const float total = ranges::accumulate(histogram, 0.F);
    HistogramStats stats;
    float count = 0;
    int bin = 0;

    // Accumulates histogram bins until the threshold is reached
    const auto accumulate_until = [&](auto threshold) {
        for (; bin < std::ssize(histogram); ++bin) {
            count += static_cast<float>(histogram[bin]);
            if (count >= threshold)
                return bin_middle_value(histogram, bin++);
        }
        return bin_middle_value(histogram, bin);
    };

    stats.p25 = accumulate_until(total * 0.25F); // NOLINT
    stats.p50 = accumulate_until(total * 0.50F); // NOLINT
    stats.p75 = accumulate_until(total * 0.75F); // NOLINT
    stats.p95 = accumulate_until(total * 0.95F); // NOLINT
    stats.p99 = accumulate_until(total * 0.99F); // NOLINT

    return stats;
}
} // namespace angonoka
