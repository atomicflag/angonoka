#pragma once

namespace angonoka::stun {
class RandomUtils {

public:
    void get_neighbor(viewi v) noexcept;
    float get_uniform() noexcept;

private:
    RandomEngine g{pcg_extras::seed_seq_from<std::random_device>{}};
    boost::random::uniform_01<float> r;

    gsl::index random_index(gsl::index max) noexcept;
    std::int_fast16_t
    pick_random(ranges::span<std::int_fast16_t> rng) noexcept;
};

} // namespace angonoka::stun
