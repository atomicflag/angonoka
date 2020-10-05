#include "stochastic_tunneling.h"
#include "random_utils.h"

#include <cmath>
#include <range/v3/algorithm/copy.hpp>
#include <utility>

namespace {
using namespace angonoka::stun;

float stun(float lowest_e, float current_e, float alpha) noexcept
{
    return 1.F - std::exp(-alpha * (current_e - lowest_e));
}

struct StochasticTunneling {
    using Counter = std::uint_fast64_t;

    gsl::not_null<RandomUtils*> random_utils;
    MakespanEstimator makespan;
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
        target_e = makespan(target_state);
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
};

} // namespace

namespace angonoka::stun {
using ::stun;
STUNResult stochastic_tunneling(
    RandomUtils& random_utils,
    MakespanEstimator&& makespan,
    span<const int16> best_state,
    Alpha alpha,
    Beta beta,
    BetaScale beta_scale) noexcept
// TODO: Figure out how to init State class
// : random_utils{std::move(random_utils)}
// , makespan{std::move(makespan)}
// // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
// , int_data{std::make_unique<int16[]>(best_state.size()*3)}
// , current_state{int_data.get(),
// static_cast<std::ptrdiff_t>(best_state.size())} ,
// target_state{int_data.get()+best_state.size(),
// static_cast<std::ptrdiff_t>(best_state.size())} ,
// best_state_{int_data.get()+best_state.size()*2,
// static_cast<std::ptrdiff_t>(best_state.size())} , alpha{alpha} ,
// beta_driver{beta, beta_scale}
{
    StochasticTunneling stoch_tun{
        .random_utils{&random_utils},
        .makespan{std::move(makespan)},
        .beta_driver{beta, beta_scale},
        .alpha{alpha}};

    // TODO: Refactor into new functions
    const auto state_size = best_state.size();
    stoch_tun.int_data.resize(state_size * 3);
    stoch_tun.current_state = {stoch_tun.int_data.data(), state_size};
    stoch_tun.target_state
        = {std::next(stoch_tun.int_data.data(), state_size),
           state_size};
    stoch_tun.best_state
        = {std::next(stoch_tun.int_data.data(), state_size * 2),
           state_size};

    ranges::copy(
        stoch_tun.best_state,
        stoch_tun.current_state.begin());
    ranges::copy(best_state, stoch_tun.best_state.begin());
    stoch_tun.current_e = stoch_tun.makespan(stoch_tun.current_state);
    stoch_tun.lowest_e = stoch_tun.current_e;
    stoch_tun.current_s = stun(
        stoch_tun.lowest_e,
        stoch_tun.current_e,
        stoch_tun.alpha);

    // for (current_iteration = 0; current_iteration < max_iterations;
    //      ++current_iteration) {
    //     get_new_neighbor();
    //     if (compare_energy_levels()) continue;
    //     perform_stun();
    // }
}

} // namespace angonoka::stun
