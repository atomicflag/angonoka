#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <cmath>
#include <cstdint>
#include <fmt/printf.h>
#include <gsl/gsl-lite.hpp>
#include <limits>
#include <memory>
#include <omp.h>
#include <pcg_random.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <random>
#include <range/v3/action/insert.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/span.hpp>
#include <thread>
#include <tuple>
#include <valgrind/callgrind.h>
#include <vector>

// TODO: Add a benchmark

namespace py {
using Int = std::int_fast16_t;
using vecf = std::vector<float>;
using viewf = ranges::span<float>;
using viewi = ranges::span<Int>;
using viewv = ranges::span<viewi>;
using veci = std::vector<Int>;
using RandomEngine = pcg32;

class RandomUtils {

public:
    // void get_random(veci& v) noexcept
    // {
    //     const auto task_count = v.size();
    //     for (gsl::index i{0}; i < task_count; ++i) {
    //         v[i] = pick_random(data.task_agents[i]);
    //     }
    // }

    inline void get_neighbor(veci& v) noexcept;

    float get_uniform() noexcept { return r(g); }

private:
    RandomEngine g{pcg_extras::seed_seq_from<std::random_device>{}};
    // RandomEngine g{0};
    boost::random::uniform_01<float> r;

    gsl::index random_index(gsl::index max) noexcept
    {
        return static_cast<gsl::index>(r(g) * max);
    }

    std::int_fast16_t
    pick_random(ranges::span<std::int_fast16_t> rng) noexcept
    {
        return rng[random_index(rng.size())];
    }
};

static_assert(std::is_nothrow_destructible_v<RandomUtils>);
static_assert(std::is_default_constructible_v<RandomUtils>);
static_assert(std::is_nothrow_copy_constructible_v<RandomUtils>);
static_assert(std::is_nothrow_copy_assignable_v<RandomUtils>);
static_assert(std::is_nothrow_move_constructible_v<RandomUtils>);
static_assert(std::is_nothrow_move_assignable_v<RandomUtils>);

int total_size(const std::vector<veci>& v)
{
    return ranges::accumulate(v, 0, [](auto acc, auto&& i) {
        return acc + i.size();
    });
}

class TaskAgents {
public:
    TaskAgents() = default;
    TaskAgents(const std::vector<veci>& data)
        : int_data{std::make_unique<Int[]>(total_size(data))}
        , spans{std::make_unique<viewi[]>(data.size())}
        , task_agents{spans.get(), static_cast<long>(data.size())}
    {
        Int* int_data_ptr = int_data.get();
        viewi* spans_ptr = spans.get();
        for (auto&& v : data) {
            *spans_ptr++
                = {int_data_ptr, static_cast<long>(v.size())};
            int_data_ptr = ranges::copy(v, int_data_ptr).out;
        }
    }

    decltype(auto) operator[](gsl::index i) const noexcept
    {
        return task_agents[i];
    }

private:
    std::unique_ptr<Int[]> int_data;
    std::unique_ptr<viewi[]> spans;
    viewv task_agents;
};

static_assert(std::is_nothrow_destructible_v<TaskAgents>);
static_assert(std::is_nothrow_default_constructible_v<TaskAgents>);
static_assert(!std::is_copy_constructible_v<TaskAgents>);
static_assert(!std::is_copy_assignable_v<TaskAgents>);
static_assert(std::is_nothrow_move_constructible_v<TaskAgents>);
static_assert(std::is_nothrow_move_assignable_v<TaskAgents>);

enum class AgentIndex : gsl::index {};
enum class TaskIndex : gsl::index {};

class TaskDurations {
public:
    TaskDurations() = default;
    TaskDurations(
        const vecf& task_durations,
        const vecf& agent_performances)
        : float_data{std::make_unique<float[]>(
            task_durations.size() * agent_performances.size())}
        , agent_count{agent_performances.size()}
    {
        for (gsl::index i{0}; i < task_durations.size(); ++i) {
            for (gsl::index j{0}; j < agent_performances.size();
                 ++j) {
                const auto idx
                    = build_index(AgentIndex{j}, TaskIndex{i});
                float_data[idx]
                    = task_durations[i] / agent_performances[j];
            }
        }
    }

    float get(AgentIndex agent, TaskIndex task) const noexcept
    {
        return float_data[build_index(agent, task)];
    }

private:
    std::unique_ptr<float[]> float_data;
    gsl::index agent_count;

    gsl::index
    build_index(AgentIndex agent, TaskIndex task) const noexcept
    {
        return static_cast<gsl::index>(task) * agent_count
            + static_cast<gsl::index>(agent);
    }
};

static_assert(std::is_nothrow_destructible_v<TaskDurations>);
static_assert(std::is_nothrow_default_constructible_v<TaskDurations>);
static_assert(!std::is_copy_constructible_v<TaskDurations>);
static_assert(!std::is_copy_assignable_v<TaskDurations>);
static_assert(std::is_nothrow_move_constructible_v<TaskDurations>);
static_assert(std::is_nothrow_move_assignable_v<TaskDurations>);

struct Data {
    TaskAgents task_agents;
    TaskDurations task_durations;

    gsl::index task_count;
    gsl::index agent_count;

    std::vector<RandomUtils> random_utils;
};

static Data data;

inline void RandomUtils::get_neighbor(veci& __restrict v) noexcept
{
    const auto task_idx = random_index(v.size());
    v[task_idx] = pick_random(data.task_agents[task_idx]);
}

void init(
    const vecf& task_durations,
    const vecf& agent_performances,
    const std::vector<veci>& task_agents)
{
    data.task_durations = {task_durations, agent_performances};
    data.task_count = task_durations.size();
    data.agent_count = agent_performances.size();
    data.task_agents = {task_agents};

    // data.random_utils.resize(0);
    data.random_utils.resize(omp_get_max_threads());
}

float stun(float lowest_e, float current_e, float alpha) noexcept
{
    // TODO: Binary search the hyperparameter
    // constexpr auto alpha = 1.2f;
    return 1.f - std::exp(-alpha * (current_e - lowest_e));
}

class MakespanEstimator {
public:
    MakespanEstimator(
        gsl::index agent_count,
        gsl::not_null<TaskDurations*> task_durations) noexcept
        : makespan_buffer_data(std::make_unique<float[]>(agent_count))
        , makespan_buffer(makespan_buffer_data.get(), agent_count)
        , task_durations{task_durations}
    {
    }

    float operator()(const veci& __restrict state) noexcept
    {
        ranges::fill(makespan_buffer, 0.f);
        const auto state_size = state.size();
        for (gsl::index i{0}; i < state_size; ++i) {
            const gsl::index a = state[i];
            makespan_buffer[a]
                += task_durations->get(AgentIndex{a}, TaskIndex{i});
        }
        return ranges::max(makespan_buffer);
    }

private:
    std::unique_ptr<float[]> makespan_buffer_data;
    ranges::span<float> makespan_buffer;
    gsl::not_null<TaskDurations*> task_durations;
};

static_assert(std::is_nothrow_destructible_v<MakespanEstimator>);
static_assert(!std::is_default_constructible_v<MakespanEstimator>);
static_assert(!std::is_copy_constructible_v<MakespanEstimator>);
static_assert(!std::is_copy_assignable_v<MakespanEstimator>);
static_assert(
    std::is_nothrow_move_constructible_v<MakespanEstimator>);
static_assert(std::is_nothrow_move_assignable_v<MakespanEstimator>);

// constexpr count_t max_iterations = 10'000'000u;
constexpr std::uint_fast64_t max_iterations = 1'000'000u;

class BetaDriver {
public:
    BetaDriver(float beta, float beta_scale)
        : value{beta}
        , beta_scale{beta_scale}
    {
    }

    void update(float stun, std::uint_fast64_t iteration) noexcept
    {
        average_stun += stun;
        if (++stun_count == average_stun_window) {
            average_stun /= stun_count;
            last_average = average_stun;
            const auto diff = average_stun - 0.03f;
            const auto t = 1.f
                - static_cast<float>(iteration) / max_iterations;
            value *= 1.f + diff * beta_scale * t * t;
            stun_count = 0u;
        }
    }

    float beta() const noexcept { return value; }

    float last_average_stun() const noexcept { return last_average; }

private:
    static constexpr std::uint_fast32_t average_stun_window
        = max_iterations / 100;

    float value;
    float average_stun{.0f};
    float last_average{.0f};
    std::uint_fast32_t stun_count{0};

    float beta_scale; // Temporary, should be hardcoded
};

static_assert(std::is_nothrow_destructible_v<BetaDriver>);
static_assert(!std::is_default_constructible_v<BetaDriver>);
static_assert(std::is_nothrow_copy_constructible_v<BetaDriver>);
static_assert(std::is_nothrow_copy_assignable_v<BetaDriver>);
static_assert(std::is_nothrow_move_constructible_v<BetaDriver>);
static_assert(std::is_nothrow_move_assignable_v<BetaDriver>);

struct OpaqueFloat {
    float value;
    operator float() { return value; }
};

struct Alpha : OpaqueFloat {
};
struct Beta : OpaqueFloat {
};
struct BetaScale : OpaqueFloat {
};

class StochasticTunneling {
public:
    StochasticTunneling(
        gsl::not_null<RandomUtils*> random_utils,
        MakespanEstimator&& makespan,
        veci&& best_state,
        Alpha alpha,
        Beta beta,
        BetaScale beta_scale)
        : random_utils{random_utils}
        , makespan{std::move(makespan)}
        , current_state{best_state}
        , best_state{std::move(best_state)}
        , target_state(current_state.size())
        , alpha{alpha}
        , beta_driver{beta, beta_scale}
    {
    }

    void run() noexcept {
        for (current_iteration = 0; current_iteration < max_iterations; ++current_iteration) {
            ranges::copy(current_state, target_state.begin());
            random_utils->get_neighbor(target_state);
            const auto target_e = makespan(target_state);
            if (__builtin_expect(target_e < lowest_e, 0)) {
                lowest_e = target_e;
                ranges::copy(target_state, best_state.begin());
                current_s = stun(lowest_e, current_e, alpha);
            }
            if (__builtin_expect(target_e < current_e, 0)) {
                current_state.swap(target_state);
                current_e = target_e;
                continue;
            }
            const float target_s = stun(lowest_e, target_e, alpha);
            const auto pr = std::min(
                1.f,
                std::exp(
                    -beta_driver.beta() * (target_s - current_s)));
            if (pr >= random_utils->get_uniform()) {
                current_state.swap(target_state);
                current_e = target_e;
                current_s = target_s;
                beta_driver.update(target_s, current_iteration);
            }
        }
    }

    // TODO: Add getters for results

private:
    using Counter = std::uint_fast64_t;

    gsl::not_null<RandomUtils*> random_utils;
    MakespanEstimator makespan;

    // TODO: Merge into single storage with a view
    veci current_state;
    veci best_state;
    veci target_state;


    float current_e{makespan(current_state)};
    float lowest_e{current_e};

    float alpha;
    float current_s{stun(lowest_e, current_e, alpha)};
    BetaDriver beta_driver;

    Counter current_iteration{0};
};

// TODO: Static asserts for StochasticTunneling

std::tuple<veci, float>
epoch(veci&& best_state, float beta, float alpha, float beta_scale)
{
    // CALLGRIND_START_INSTRUMENTATION;
    // CALLGRIND_TOGGLE_COLLECT;
    veci global_state;
    float global_e;
    float global_beta = 0.f;

#pragma omp parallel firstprivate(best_state, beta)
    {
        using count_t = std::uint_fast64_t;

        veci current_state = best_state;
        veci target_state(current_state.size());

        MakespanEstimator makespan{
            data.agent_count,
            &data.task_durations};
        float current_e = makespan(current_state);
        float lowest_e = current_e;

        float current_s = stun(lowest_e, current_e, alpha);
        BetaDriver beta_driver{beta, beta_scale};

        auto& __restrict random_utils
            = data.random_utils[omp_get_thread_num()];
        for (count_t i{0}; i < max_iterations; ++i) {
            // target_state = current_state;
            ranges::copy(current_state, target_state.begin());
            random_utils.get_neighbor(target_state);
            const auto target_e = makespan(target_state);
            if (__builtin_expect(target_e < lowest_e, 0)) {
                lowest_e = target_e;
                // best_state = target_state;
                ranges::copy(target_state, best_state.begin());
                current_s = stun(lowest_e, current_e, alpha);
                // fmt::print("{}\n", lowest_e);
            }
            if (__builtin_expect(target_e < current_e, 0)) {
                current_state.swap(target_state);
                current_e = target_e;
                continue;
            }
            const float target_s = stun(lowest_e, target_e, alpha);
            const auto pr = std::min(
                1.f,
                std::exp(
                    -beta_driver.beta() * (target_s - current_s)));
            if (pr >= random_utils.get_uniform()) {
                current_state.swap(target_state);
                current_e = target_e;
                current_s = target_s;
                beta_driver.update(target_s, i);
            }
        }
        // fmt::print("average_stun: {}\n", last_average_stun);
#pragma omp critical
        {
            if (global_state.empty() || lowest_e < global_e) {
                global_state = std::move(best_state);
                global_e = lowest_e;
            }
            global_beta += beta_driver.beta();
        }
    }
    // CALLGRIND_START_INSTRUMENTATION;
    // CALLGRIND_TOGGLE_COLLECT;
    return {
        std::move(global_state),
        global_beta / static_cast<float>(omp_get_max_threads())};
}
} // namespace py

PYBIND11_MODULE(libangonoka, m)
{
    m.def(
        "init",
        &py::init,
        pybind11::call_guard<pybind11::gil_scoped_release>());
    m.def(
        "epoch",
        &py::epoch,
        pybind11::call_guard<pybind11::gil_scoped_release>());
    // m.def("get_random", &py::get_rand);
    // m.def("makespan", &py::makespan);
}
