#include <catch2/catch.hpp>

#include <cmath>

namespace angonoka::stun {
// TODO: doc, test, expects
// TODO: name? ExpCurveFitter?
class ProgressEstimator {
public:
    // TODO: doc, test, expects
    void reset() noexcept { *this = ProgressEstimator(); }

    // TODO: doc, test, expects
    float operator()(float x, float y) noexcept
    {
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

TEST_CASE("ProgressEstimator")
{
    using angonoka::stun::ProgressEstimator;
    ProgressEstimator est;

    REQUIRE(est(0.F, 0.F) == Approx(0.F));
    est.reset();
    est(1.F, .011108996538242F);
    est(2.F, .018315638888734F);
    est(3.F, .030197383422319F);
    est(4.F, .049787068367864F);
    REQUIRE(est(5.F, .082084998623899F) == Approx(.082084998623899F));
    est(6.F, .135335283236613F);
    est(7.F, .22313016014843F);
    est(8.F, .367879441171442F);
    est(9.F, .606530659712634F);
    REQUIRE(est(10.F, 1.F) == Approx(1.F));
    est.reset();
    REQUIRE(est(0.F, 0.F) == Approx(0.F));
}
