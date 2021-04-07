#include "stun/exp_curve_fitter.h"
#include <catch2/catch.hpp>

TEST_CASE("ExpCurveFitter type traits")
{
    using angonoka::stun::ExpCurveFitter;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<ExpCurveFitter>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<ExpCurveFitter>);
    STATIC_REQUIRE(

        std::is_nothrow_copy_constructible_v<ExpCurveFitter>);
    STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<ExpCurveFitter>);
    STATIC_REQUIRE(
        std::is_nothrow_move_constructible_v<ExpCurveFitter>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<ExpCurveFitter>);
}

TEST_CASE("ExpCurveFitter basic operations")
{
    using angonoka::stun::ExpCurveFitter;
    ExpCurveFitter fit;

    REQUIRE(fit(0.F, 0.F) == Approx(0.F));
    fit.reset();
    fit(1.F, .011108996538242F);
    fit(2.F, .018315638888734F);
    fit(3.F, .030197383422319F);
    fit(4.F, .049787068367864F);
    REQUIRE(fit(5.F, .082084998623899F) == Approx(.082084998623899F));
    fit(6.F, .135335283236613F);
    fit(7.F, .22313016014843F);
    fit(8.F, .367879441171442F);
    fit(9.F, .606530659712634F);
    REQUIRE(fit(10.F, 1.F) == Approx(1.F));
    fit.reset();
    REQUIRE(fit(0.F, 0.F) == Approx(0.F));
}
