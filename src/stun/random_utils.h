#pragma once

#include "common.h"
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <gsl/gsl-lite.hpp>
#include <pcg_random.hpp>
#include <random>

namespace angonoka::stun {
using RandomEngine = pcg32;

/**
    Miscellaneous random number generators.
*/
class RandomUtils {
public:
    /**
        Default constructor.
    */
    RandomUtils();

    /**
        Constructor with a fixed PRNG seed.

        @param seed Random engine seed
    */
    RandomUtils(gsl::index seed);

    /**
        Uniformally distributed real value between 0 and 1.

        @return Random number
    */
    float uniform_01() noexcept;

    /**
        Uniformally distributed discrete value between 0 and max.

        @param max Maximum value

        @return Random number
    */
    int16 uniform_int(int16 max) noexcept;

private:
    RandomEngine generator{
        pcg_extras::seed_seq_from<std::random_device>{}};
    boost::random::uniform_01<float> uniform_01_;
    boost::random::uniform_int_distribution<std::int_fast16_t>
        uniform_int_;
};
} // namespace angonoka::stun
