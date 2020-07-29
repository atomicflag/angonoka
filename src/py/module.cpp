#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <cstdint>
#include <fmt/printf.h>
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <pcg_random.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <random>
#include <range/v3/action/insert.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/span.hpp>
#include <tuple>
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

struct Data {
    std::unique_ptr<float[]> float_data;
    std::unique_ptr<Int[]> int_data;
    std::unique_ptr<viewi[]> spans;

    viewf task_durations;
    viewf agent_performances;

    viewv agent_tasks;
    viewv task_agents;
};

viewf insert_data(const vecf in, float* data)
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

static Data data;

void init(
    vecf&& task_durations,
    vecf&& agent_performances,
    std::vector<veci>&& task_agents)
{
    const auto agents_tasks_count
        = task_durations.size() + agent_performances.size();
    data.float_data = std::make_unique<float[]>(agents_tasks_count);
    data.int_data = std::make_unique<Int[]>(total_size(task_agents));
    data.spans = std::make_unique<viewi[]>(agents_tasks_count);

    data.task_durations
        = insert_data(task_durations, data.float_data.get());
    data.agent_performances
        = insert_data(agent_performances, data.task_durations.end());

    flatten(task_agents, data.int_data.get(), data.spans.get());

    data.task_agents
        = {data.spans.get(),
           static_cast<long>(task_agents.size())};
}

struct Generator {
    RandomEngine g{pcg_extras::seed_seq_from<std::random_device>{}};
    boost::random::uniform_01<float> r;

    gsl::index
    random_index (gsl::index max) noexcept {
        return static_cast<gsl::index>(r(g) * max);
    }

    std::int_fast16_t
    pick_random(ranges::span<int_fast16_t> rng) noexcept
    {
        return rng[random_index(rng.size())];
    }

    // void get_random(veci& v) noexcept
    // {
    //     const auto task_count = v.size();
    //     for (gsl::index i{0}; i < task_count; ++i) {
    //         v[i] = pick_random(data.task_agents[i]);
    //     }
    // }

    void get_neighbor(veci& v) noexcept
    {
        const auto task_idx = random_index(v.size());
        v[task_idx] = pick_random(data.task_agents[task_idx]);
    }
};

float makespan(const veci& v)
{
    thread_local vecf buf;
    buf.resize(data.agent_performances.size());
    ranges::fill(buf, 0.f);
    const auto task_count = v.size();
    for (gsl::index i{0}; i < task_count; ++i) {
        const auto agent = v[i];
        buf[agent] += data.task_durations[i]
            / data.agent_performances[agent];
    }
    return ranges::max(buf);
}

float stun(float low, float e)
{
    const auto alpha = 5.f;
    return 1.f - __builtin_expf(-alpha * (e - low));
}

std::tuple<veci, float, float>
epoch(veci&& start, float e_start, float beta_start)
{
    using count_t = std::uint_fast64_t;
    float beta = beta_start;
    veci cur{std::move(start)}, best, buf;
    float e = e_start;
    float low = e;
    best = cur;
    float avg_stun = 0.f;
    count_t stun_count = 0;
    constexpr count_t max_c = 10000000u;
    float last_avg_stun = 0.f;
    // TODO: gen shouldn't be constructed here
    Generator gen;
    for (count_t i{0}; i < max_c; ++i) {
        buf = cur;
        gen.get_neighbor(buf);
        const auto e2 = makespan(buf);
        if (e2 < low) {
            low = e2;
            best = buf;
            fmt::print("{}\n", low);
        }
        const auto s2 = stun(low, e2);
        const auto s1 = stun(low, e);
        const auto pr
            = __builtin_fminf(1.f, __builtin_expf(-beta * (s2 - s1)));
        if (stun_count >= 100000) {
            avg_stun /= stun_count;
            last_avg_stun = avg_stun;
            const auto diff = avg_stun - 0.03f;
            beta *= 1.f+diff*__builtin_powf(1.f-static_cast<float>(i)/max_c, 2.f);
            beta = __builtin_fminf(beta, 1e6f);
            // fmt::print("beta {}\n", 1.f+(avg_stun-0.03f)*__builtin_powf(1.f-static_cast<float>(i)/max_c, 2.f));
            stun_count = 0u;
        }
        if (e2 < e || pr >= gen.r(gen.g)) {
            cur = buf;
            e = e2;
            avg_stun += s2;
            ++stun_count;
        }
    }
    fmt::print("avg_stun: {}\n", last_avg_stun);
    return {std::move(best), low, beta};
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
