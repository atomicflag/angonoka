#include "stochastic_tunneling.h"
#include "common.h"
#include "random_utils.h"
#include <cmath>
#include <range/v3/algorithm/copy.hpp>
#include <utility>

namespace {
using namespace angonoka::stun;
using angonoka::stun::index;

float stun(float lowest_e, float current_e, float alpha) noexcept
{
    return 1.F - std::exp(-alpha * (current_e - lowest_e));
}

struct StochasticTunneling {
    using Counter = std::uint_fast64_t;

    gsl::not_null<RandomUtils*> random_utils;
    gsl::not_null<MakespanEstimator*> makespan;
    BetaDriver beta_driver;

    std::vector<int16> int_data;
    span<int16> current_state;
    span<int16> target_state;
    span<int16> best_state;

    float current_e;
    float lowest_e;
    float target_e;

    float alpha; // TODO: Should be a constant
    float current_s;
    float target_s;

    std::uint_fast64_t current_iteration{0};

    void get_new_neighbor() noexcept
    {
        ranges::copy(current_state, target_state.begin());
        random_utils->get_neighbor_inplace(target_state);
        target_e = (*makespan)(target_state);
    }

    bool compare_energy_levels() noexcept
    {
        if (target_e < current_e) {
            if (target_e < lowest_e) {
                lowest_e = target_e;
                ranges::copy(target_state, best_state.begin());
                current_s = stun(lowest_e, current_e, alpha);
            }
            std::swap(current_state, target_state);
            current_e = target_e;
            return true; // TODO: Enum
        }
        return false;
    }

    void perform_stun() noexcept
    {
        target_s = stun(lowest_e, target_e, alpha);
        const auto delta_s = target_s - current_s;
        const auto pr
            = std::min(1.F, std::exp(-beta_driver.beta() * delta_s));
        if (pr >= random_utils->get_uniform()) {
            std::swap(current_state, target_state);
            current_e = target_e;
            current_s = target_s;
            beta_driver.update(target_s, current_iteration);
        }
    }

    void copy_best_state(span<const int16> source_state)
    {
        ranges::copy(source_state, best_state.begin());
        ranges::copy(source_state, current_state.begin());
    }

    void init_states(index state_size)
    {
        int_data.resize(static_cast<gsl::index>(state_size) * 3);
        auto* const data = int_data.data();
        const auto next = [&] {
            const auto* const next = std::next(data, state_size);
            return std::exchange(data, next);
        };
        current_state = {next(), state_size};
        target_state = {next(), state_size};
        best_state = {next(), state_size};
    }

    void init_energies()
    {
        current_e = (*makespan)(current_state);
        lowest_e = current_e;
        current_s = stun(lowest_e, current_e, alpha);
    }
};

} // namespace

namespace angonoka::stun {
using ::stun;
STUNResult stochastic_tunneling(
    RandomUtils& random_utils,
    MakespanEstimator& makespan,
    span<const int16> best_state,
    Alpha alpha,
    Beta beta,
    BetaScale beta_scale)
{
    StochasticTunneling stun_op{
        .random_utils{&random_utils},
        .makespan{&makespan},
        .beta_driver{beta, beta_scale},
        .alpha{alpha}};

    stun_op.init_states(best_state.size());
    stun_op.copy_best_state(best_state);
    stun_op.init_energies();

    // for (current_iteration = 0; current_iteration < max_iterations;
    //      ++current_iteration) {
    //     get_new_neighbor();
    //     if (compare_energy_levels()) continue;
    //     perform_stun();
    // }
}

} // namespace angonoka::stun
