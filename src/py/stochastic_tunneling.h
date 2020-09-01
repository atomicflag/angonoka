#pragma once

namespace angonoka::stun {
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
        BetaScale beta_scale);

    void run() noexcept;

    float lowest_e() const noexcept;

    viewi best_state() const noexcept;

    float beta() const noexcept;

private:
    using Counter = std::uint_fast64_t;

    gsl::not_null<RandomUtils*> random_utils;
    MakespanEstimator makespan;

    std::unique_ptr<Int[]> int_data;
    viewi current_state;
    viewi target_state;
    viewi best_state_;

    float current_e;
    float lowest_e_;
    float target_e;

    float alpha; // Should be a constant
    float current_s{stun(lowest_e_, current_e, alpha)};
    float target_s;
    BetaDriver beta_driver;

    Counter current_iteration{0};

    void get_new_neighbor() noexcept;

    bool compare_energy_levels() noexcept;

    void perform_stun() noexcept;
};
} // namespace angonoka::stun
