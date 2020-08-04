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

struct Generator {
    RandomEngine g{pcg_extras::seed_seq_from<std::random_device>{}};
    boost::random::uniform_01<float> r;

    // void get_random(veci& v) noexcept
    // {
    //     const auto task_count = v.size();
    //     for (gsl::index i{0}; i < task_count; ++i) {
    //         v[i] = pick_random(data.task_agents[i]);
    //     }
    // }

    inline gsl::index random_index(gsl::index max) noexcept;

    inline std::int_fast16_t
    pick_random(ranges::span<int_fast16_t> rng) noexcept;

    inline void get_neighbor(veci& v) noexcept;
};

enum class AgentIndex : gsl::index {};
enum class TaskIndex : gsl::index {};

struct Data {
    std::unique_ptr<float[]> float_data;
    std::unique_ptr<Int[]> int_data;
    std::unique_ptr<viewi[]> spans;

    viewv agent_tasks;
    viewv task_agents;

    gsl::index task_count;
    gsl::index agent_count;

    gsl::index get_task_duration_index(
        AgentIndex agent,
        TaskIndex task) const noexcept
    {
        return static_cast<gsl::index>(task) * agent_count
            + static_cast<gsl::index>(agent);
    }

    void
    set_task_duration_for(AgentIndex agent, TaskIndex task, float value)
    {
        float_data[get_task_duration_index(agent, task)] = value;
    }

    float get_task_duration_for(AgentIndex agent, TaskIndex task)
        const noexcept
    {
        return float_data[get_task_duration_index(agent, task)];
    }

    std::vector<Generator> gens;
};

static Data data;

inline gsl::index Generator::random_index(gsl::index max) noexcept
{
    return static_cast<gsl::index>(r(g) * max);
}

inline std::int_fast16_t
Generator::pick_random(ranges::span<int_fast16_t> rng) noexcept
{
    return rng[random_index(rng.size())];
}

inline void Generator::get_neighbor(veci& __restrict v) noexcept
{
    const auto task_idx = random_index(v.size());
    v[task_idx] = pick_random(data.task_agents[task_idx]);
}

viewf insert_data(const vecf& in, float* data)
{
    ranges::copy(in, data);
    return {data, static_cast<long>(in.size())};
}

auto flatten(const std::vector<veci>& in, Int* data, viewi* spans)
{
    for (auto&& v : in) {
        *spans = {data, static_cast<long>(v.size())};
        ++spans;
        data = ranges::copy(v, data).out;
    }
    return std::make_tuple(data, spans);
}

int total_size(const std::vector<veci>& v)
{
    return ranges::accumulate(v, 0, [](auto acc, auto&& i) {
        return acc + i.size();
    });
}

void init(
    vecf&& task_durations,
    vecf&& agent_performances,
    std::vector<veci>&& task_agents)
{
    const auto task_duration_count
        = task_durations.size() * agent_performances.size();
    data.float_data = std::make_unique<float[]>(task_duration_count);
    data.int_data = std::make_unique<Int[]>(total_size(task_agents));
    data.spans = std::make_unique<viewi[]>(
        task_durations.size() + agent_performances.size());
    data.task_count = task_durations.size();
    data.agent_count = agent_performances.size();

    for (gsl::index i{0}; i < task_durations.size(); ++i) {
        for (gsl::index j{0}; j < agent_performances.size(); ++j) {
            data.set_task_duration_for(
                AgentIndex{j},
                TaskIndex{i},
                task_durations[i] / agent_performances[j]);
        }
    }

    flatten(task_agents, data.int_data.get(), data.spans.get());

    data.task_agents
        = {data.spans.get(), static_cast<long>(task_agents.size())};
    data.gens.resize(omp_get_max_threads());
}

float stun(float lowest_e, float current_e) noexcept
{
    constexpr auto alpha = 5.f;
    return 1.f - std::exp(-alpha * (current_e - lowest_e));
}

enum class AgentCount : gsl::index {};

class SchedulingUtils {
public:
    SchedulingUtils(
        AgentCount agent_count,
        gsl::not_null<Data*> data) noexcept
        : makespan_buffer(static_cast<gsl::index>(agent_count))
        , data{data}
    {
    }

    float makespan_of(const veci& __restrict state) noexcept
    {
        ranges::fill(makespan_buffer, 0.f);
        const auto state_size = state.size();
        for (gsl::index i{0}; i < state_size; ++i) {
            const gsl::index a = state[i];
            makespan_buffer[a] += data->get_task_duration_for(
                AgentIndex{a},
                TaskIndex{i});
        }
        return ranges::max(makespan_buffer);
    }

private:
    vecf makespan_buffer;
    gsl::not_null<Data*> data;
};

std::tuple<veci, float> epoch(veci&& best_state, float beta)
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

        SchedulingUtils utils{AgentCount{data.agent_count}, &data};
        float current_e = utils.makespan_of(current_state);
        float lowest_e = current_e;

        float current_s = stun(lowest_e, current_e);
        float average_stun = 0.f;
        count_t stun_count = 0u;
        float last_average_stun;

        auto& __restrict gen = data.gens[omp_get_thread_num()];
        constexpr count_t max_iterations = 10'000'000u;
        for (count_t i{0}; i < max_iterations; ++i) {
            // target_state = current_state;
            ranges::copy(current_state, target_state.begin());
            gen.get_neighbor(target_state);
            const auto target_e = utils.makespan_of(target_state);
            if (__builtin_expect(target_e < lowest_e, 0)) {
                lowest_e = target_e;
                // best_state = target_state;
                ranges::copy(target_state, best_state.begin());
                current_s = stun(lowest_e, current_e);
                fmt::print("{}\n", lowest_e);
            }
            if (__builtin_expect(target_e < current_e, 0)) {
                current_state.swap(target_state);
                current_e = target_e;
                continue;
            }
            const float target_s = stun(lowest_e, target_e);
            const auto pr = std::min(
                1.f,
                std::exp(-beta * (target_s - current_s)));
            if (pr >= gen.r(gen.g)) {
                current_state.swap(target_state);
                current_e = target_e;
                current_s = target_s;
                average_stun += target_s;
                ++stun_count;
            }
            // TODO: Move into Beta (temp) scheduler?
            constexpr auto average_stun_window = 100'000u;
            if (stun_count >= average_stun_window) {
                average_stun /= stun_count;
                last_average_stun = average_stun;
                const auto diff = average_stun - 0.03f;
                const auto t
                    = 1.f - static_cast<float>(i) / max_iterations;
                beta *= 1.f + diff * .1f * t * t;
                stun_count = 0u;
            }
        }
        fmt::print("average_stun: {}\n", last_average_stun);
#pragma omp critical
        {
            if (global_state.empty() || lowest_e < global_e) {
                global_state = std::move(best_state);
                global_e = lowest_e;
            }
            global_beta += beta;
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
