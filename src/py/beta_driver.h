#pragma once

namespace angonoka::stun {
// constexpr std::uint_fast64_t max_iterations = 1'000'000u;
constexpr std::uint_fast64_t max_iterations = 10'000'000u;

class BetaDriver {
public:
    BetaDriver(float beta, float beta_scale);

    void update(float stun, std::uint_fast64_t iteration) noexcept;

    float beta() const noexcept;

    float last_average_stun() const noexcept;

private:
    static constexpr std::uint_fast32_t average_stun_window
        = max_iterations / 100;

    float value;
    float average_stun{.0f};
    float last_average{.0f};
    std::uint_fast32_t stun_count{0};

    float beta_scale; // Temporary, should be hardcoded
};
} // namespace angonoka::stun
