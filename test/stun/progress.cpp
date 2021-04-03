#include <catch2/catch.hpp>

namespace angonoka::stun {
// TODO: doc, test, expects
class ProgressEstimator {
public:
    // TODO: doc, test, expects
    void reset() noexcept;

    // TODO: doc, test, expects
    float operator()(float x, float y) noexcept;

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
    ProgressEstimator p_estimator;
    // TODO: tests
}
