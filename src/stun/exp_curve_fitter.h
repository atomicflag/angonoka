#pragma once

namespace angonoka::stun {
/**
    Online estimation of an exponential curve from noisy input.
*/
class ExpCurveFitter {
public:
    /**
        Reset the internal state.
    */
    void reset() noexcept;

    /**
        Update the estimate.

        @param x Coordinate along the first axis
        @param y Coordinate along the second axis

        @return New estimate based on the latest data.
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
