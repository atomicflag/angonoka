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

int total_size(const std::vector<veci>& v)
{
    return ranges::accumulate(v, 0, [](auto acc, auto&& i) {
        return acc + i.size();
    });
}

struct Data {
    TaskAgents task_agents;
    TaskDurations task_durations;

    gsl::index task_count;
    gsl::index agent_count;

    std::vector<RandomUtils> random_utils;
};

static Data data;

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
        StochasticTunneling stochastic_tunneling{
            &data.random_utils[omp_get_thread_num()],
            {data.agent_count, &data.task_durations},
            std::move(best_state),
            Alpha{alpha},
            Beta{beta},
            BetaScale{beta_scale}};

        stochastic_tunneling.run();

#pragma omp critical
        {
            const auto lowest_e = stochastic_tunneling.lowest_e();
            if (global_state.empty() || lowest_e < global_e) {
                global_state = stochastic_tunneling.best_state()
                    | ranges::to<veci>();
                global_e = lowest_e;
            }
            global_beta += stochastic_tunneling.beta();
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
