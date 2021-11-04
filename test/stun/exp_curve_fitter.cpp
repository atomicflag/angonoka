#include "stun/exp_curve_fitter.h"
#include <catch2/catch.hpp>

TEST_CASE("exponential curve fitting")
{
    SECTION("ExpCurveFitter type traits")
    {
        using angonoka::stun::ExpCurveFitter;
        STATIC_REQUIRE(
            std::is_nothrow_destructible_v<ExpCurveFitter>);
        STATIC_REQUIRE(
            std::is_nothrow_default_constructible_v<ExpCurveFitter>);
        STATIC_REQUIRE(

            std::is_nothrow_copy_constructible_v<ExpCurveFitter>);
        STATIC_REQUIRE(
            std::is_nothrow_copy_assignable_v<ExpCurveFitter>);
        STATIC_REQUIRE(
            std::is_nothrow_move_constructible_v<ExpCurveFitter>);
        STATIC_REQUIRE(
            std::is_nothrow_move_assignable_v<ExpCurveFitter>);
    }

    SECTION("ExpCurveFitter basic operations")
    {
        using angonoka::stun::ExpCurveFitter;
        ExpCurveFitter fit;

        REQUIRE(fit(0.F, 0.F) == Approx(0.));
        fit.reset();

        // exp(x / 2 - 5 * log(10))

        fit(1.F, .0111089965382F);
        fit(2.F, .0183156388887F);
        fit(3.F, .0301973834223F);
        fit(4.F, .0497870683679F);
        REQUIRE(fit(5.F, .0820849986239F) == Approx(.0820849986239));
        fit(6.F, .135335283237F);
        fit(7.F, .223130160148F);
        fit(8.F, .367879441171F);
        fit(9.F, .606530659713F);
        REQUIRE(fit(10.F, 1.F) == Approx(1.));

        REQUIRE(fit.at(10.F) == Approx(1.));
        REQUIRE(fit.at(0.F) == Approx(.00673794699909));
        REQUIRE(fit.at(1.F) == Approx(.0111089965382));

        fit.reset();

        REQUIRE(fit(0.F, 0.F) == Approx(0.));
    }
}
