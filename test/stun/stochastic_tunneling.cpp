#include "stun/stochastic_tunneling.h"
#include "stun/makespan_estimator.h"
#include "stun/random_utils.h"
#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>
#include <vector>

namespace {
using namespace angonoka::stun;
struct RandomUtilsMock final : RandomUtilsStub {
    MAKE_MOCK1(
        get_neighbor_inplace,
        void(span<int16>),
        noexcept override);
    MAKE_MOCK0(get_uniform, float(), noexcept override);
};

struct MakespanEstimatorMock final : MakespanEstimatorStub {
    float operator()(span<const int16> state) noexcept override
    {
        return call(state);
    }
    MAKE_MOCK1(call, float(span<const int16>));
};
} // namespace

TEST_CASE("Stochastic tunnleing")
{
    using trompeloeil::_;
    using namespace angonoka::stun;

    std::vector<int16> best_state{0, 0};

    RandomUtilsMock random_utils;
    REQUIRE_CALL(random_utils, get_neighbor_inplace(_))
        .WITH(static_cast<gsl::index>(_1.size()) == best_state.size())
        .TIMES(2);
    REQUIRE_CALL(random_utils, get_uniform()).RETURN(1.F).TIMES(2);

    MakespanEstimatorMock estimator;
    REQUIRE_CALL(estimator, call(_))
        .WITH(static_cast<gsl::index>(_1.size()) == best_state.size())
        .RETURN(1.F)
        .TIMES(3);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
    const auto result = stochastic_tunneling(
        random_utils,
        estimator,
        best_state,
        Alpha{.5F},
        Beta{1.F},
        BetaScale{.3F});
#pragma clang diagnostic pop

    REQUIRE(result.lowest_e == Approx(1.0F));
}
