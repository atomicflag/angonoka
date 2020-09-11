#pragma once

#include "common.h"
#include <boost/random/uniform_01.hpp>
#include <gsl/gsl-lite.hpp>
#include <pcg_random.hpp>
#include <random>
#include <range/v3/view/span.hpp>

namespace angonoka::stun {
using ranges::span;
using RandomEngine = pcg32;

class RandomUtils {
public:
    void get_neighbor(span<int16> v) noexcept;
    float get_uniform() noexcept;


private:
    RandomEngine g{pcg_extras::seed_seq_from<std::random_device>{}};
    boost::random::uniform_01<float> r;
    using index_type = span<int16>::index_type;

    index_type random_index(index_type max) noexcept;
    int16 pick_random(span<const int16> rng) noexcept;
};
} // namespace angonoka::stun
