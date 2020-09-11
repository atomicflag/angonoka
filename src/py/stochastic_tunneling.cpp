#include "stochastic_tunneling.h"
#include "random_utils.h"

#include <cmath>
#include <range/v3/algorithm/copy.hpp>
#include <utility>

namespace {
float stun(float lowest_e, float current_e, float alpha) noexcept
{
    return 1.F - std::exp(-alpha * (current_e - lowest_e));
}
} // namespace angonoka::stun::detail

namespace angonoka::stun {
    using ::stun;
StochasticTunneling::StochasticTunneling(
        gsl::not_null<RandomUtils*> random_utils,
        MakespanEstimator&& makespan,
        span<const int16> best_state,
        Alpha alpha,
        Beta beta,
        BetaScale beta_scale)
        : random_utils{std::move(random_utils)}
        , makespan{std::move(makespan)}
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
        , int_data{std::make_unique<int16[]>(best_state.size()*3)}
        , current_state{int_data.get(), static_cast<std::ptrdiff_t>(best_state.size())}
        , target_state{int_data.get()+best_state.size(), static_cast<std::ptrdiff_t>(best_state.size())}
        , best_state_{int_data.get()+best_state.size()*2, static_cast<std::ptrdiff_t>(best_state.size())}
        , alpha{alpha}
        , beta_driver{beta, beta_scale}
{
    ranges::copy(best_state, current_state.begin());
    ranges::copy(best_state, best_state_.begin());
    current_e = makespan(current_state);
    lowest_e_ = current_e;
    current_s = stun(lowest_e_, current_e, alpha);
}

void StochasticTunneling::get_new_neighbor() noexcept
{
    ranges::copy(current_state, target_state.begin());
    random_utils->get_neighbor(target_state);
    target_e = makespan(target_state);
}

bool StochasticTunneling::compare_energy_levels() noexcept
{
    if (target_e < current_e) {
        if (target_e < lowest_e_) {
            lowest_e_ = target_e;
            ranges::copy(target_state, best_state_.begin());
            current_s = stun(lowest_e_, current_e, alpha);
        }
        std::swap(current_state, target_state);
        current_e = target_e;
        return true;
    }
    return false;
}

void StochasticTunneling::perform_stun() noexcept
{
    target_s = stun(lowest_e_, target_e, alpha);
    const auto pr = std::min(
        1.F,
        std::exp(-beta_driver.beta() * (target_s - current_s)));
    if (pr >= random_utils->get_uniform()) {
        std::swap(current_state, target_state);
        current_e = target_e;
        current_s = target_s;
        beta_driver.update(target_s, current_iteration);
    }
}

void StochasticTunneling::run() noexcept
{
    for (current_iteration = 0; current_iteration < max_iterations;
         ++current_iteration) {
        get_new_neighbor();
        if (compare_energy_levels()) continue;
        perform_stun();
    }
}

float StochasticTunneling::lowest_e() const noexcept
{
    return lowest_e_;
}

span<const int16> StochasticTunneling::best_state() const noexcept
{
    return best_state_;
}

float StochasticTunneling::beta() const noexcept
{
    return beta_driver.beta();
}
} // namespace angonoka::stun
