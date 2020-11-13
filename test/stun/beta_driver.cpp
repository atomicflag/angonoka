#include "stun/beta_driver.h"
#include <catch2/catch.hpp>

TEST_CASE("BetaDriver")
{
    using namespace angonoka::stun;

    BetaDriver beta{1.f, 0.5F};
    const auto starting_beta = beta.beta();

    SECTION("Increase")
    {
        for (unsigned i{0}; i < 500'000u; ++i) beta.update(5.F, i);
        REQUIRE(beta.beta() > starting_beta);
        REQUIRE(beta.last_average_stun() == Approx(5.F));
    }

    SECTION("Decrease")
    {
        for (unsigned i{0}; i < 500'000u; ++i) beta.update(.01F, i);
        REQUIRE(beta.beta() < starting_beta);
        REQUIRE(
            beta.last_average_stun() == Approx(.01F).margin(0.001F));
    }
}

TEST_CASE("BetaDriver type traits")
{
    using angonoka::stun::BetaDriver;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<BetaDriver>);
    STATIC_REQUIRE(!std::is_default_constructible_v<BetaDriver>);
    STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<BetaDriver>);
    STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<BetaDriver>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<BetaDriver>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<BetaDriver>);
}
