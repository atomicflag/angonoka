#include <catch2/catch.hpp>

#include <cmath>
#include <gsl/gsl-lite.hpp>

namespace angonoka::stun {
/**
    Online estimation of an exponential curve from noisy input.
*/
class ExpCurveFitter {
public:
    /**
        Reset the internal state.
    */
    void reset() noexcept
    {
        *this = ExpCurveFitter();

        Ensures(xxy == 0);
        Ensures(ylogy == 0);
        Ensures(xy == 0);
        Ensures(xylogy == 0);
        Ensures(sumy == 0);
    }

    /**
        Update the estimate.

        @param x Coordinate along the first axis
        @param y Coordinate along the second axis

        @return New estimate based on the latest data.
    */
    float operator()(float x, float y) noexcept
    {
        // Not strictly necessary
        Expects(x >= 0.F);
        Expects(y >= 0.F);

        const auto xy1 = x * y;
        const auto ylogy1 = y * std::log(y);
        xxy += x * xy1;
        ylogy += ylogy1;
        xy += xy1;
        xylogy += x * ylogy1;
        sumy += y;
        const auto divisor = sumy * xxy - std::pow(xy, 2.F);
        if (divisor == 0.F) return 0.F;
        const auto a = (xxy * ylogy - xy * xylogy) / divisor;
        const auto b = (sumy * xylogy - xy * ylogy) / divisor;
        return std::exp(b * x + a);
    }

private:
    float xxy = 0;
    float ylogy = 0;
    float xy = 0;
    float xylogy = 0;
    float sumy = 0;
};
} // namespace angonoka::stun

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
