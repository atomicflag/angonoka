#include "stun/temperature.h"
#include <catch2/catch.hpp>

TEST_CASE("Temperature type traits")
{
    using angonoka::stun::Temperature;
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Temperature>);
    STATIC_REQUIRE(!std::is_default_constructible_v<Temperature>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Temperature>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Temperature>);
    STATIC_REQUIRE(std::is_move_constructible_v<Temperature>);
    STATIC_REQUIRE(std::is_move_assignable_v<Temperature>);
}

TEST_CASE("Temperature update")
{
    using namespace angonoka::stun;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
    Temperature temp{
        Beta{1.f},
        BetaScale{1e-3f},
        StunWindow{5},
        RestartPeriod{64}};
#pragma clang diagnostic pop

    REQUIRE(temp == 1.f);

    for (int i{0}; i < 10; ++i) temp.update(1.f);

    REQUIRE(temp.average_stun() == Approx(1.f));
    REQUIRE(temp > 1.f);

    for (int i{0}; i < 1000; ++i) temp.update(0.f);

    REQUIRE(temp.average_stun() == Approx(0.f).margin(0.001));
    REQUIRE(temp < 1.f);
}
