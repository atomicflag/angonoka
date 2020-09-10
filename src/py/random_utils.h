#pragma once

#include <boost/random/uniform_01.hpp>
#include <gsl/gsl-lite.hpp>
#include <range/v3/view/span.hpp>
#include "common.h"
#include <pcg_random.hpp>

namespace angonoka::stun {
    using ranges::span;
class RandomUtils {

public:
    void get_neighbor(span<int16> v) noexcept;
    float get_uniform() noexcept;

private:
    RandomEngine g{pcg_extras::seed_seq_from<std::random_device>{}}; // init in tu?
    boost::random::uniform_01<float> r;

    gsl::index random_index(gsl::index max) noexcept;
    int16
    pick_random(span<const int16> rng) noexcept;
};

} // namespace angonoka::stun
