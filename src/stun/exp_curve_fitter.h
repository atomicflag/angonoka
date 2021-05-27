#pragma once

namespace angonoka::stun {
/**
    Online estimation of an exponential curve from noisy input.

    Currently used for estimating progress during stochastic tunneling
    optimization. This class allows for fitting an exponential curve
    without requiring an array of the historical values.
*/
class ExpCurveFitter {
public:
    /**
        Reset the internal state.
    */
    void reset() noexcept;

    /**
        Estimate the curve's value at a point x.

        Fits the exponential curve over all previous and current
        data points and returns an estimated value for the point x.

        @param x Coordinate along the x axis
        @param y Value of the exponential function

        @return Estimated value of f(x)
    */
    float operator()(float x, float y) noexcept;

private:
    float xxy = 0;
    float ylogy = 0;
    float xy = 0;
    float xylogy = 0;
    float sumy = 0;
};
} // namespace angonoka::stun
