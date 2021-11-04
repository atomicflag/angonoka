#include "stun/temperature.h"
#include <catch2/catch.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"

TEST_CASE("Temperature")
{
    SECTION("Temperature type traits")
    {
        using angonoka::stun::Temperature;
        STATIC_REQUIRE(std::is_nothrow_destructible_v<Temperature>);
        STATIC_REQUIRE_FALSE(
            std::is_default_constructible_v<Temperature>);
        STATIC_REQUIRE(std::is_copy_constructible_v<Temperature>);
        STATIC_REQUIRE(std::is_copy_assignable_v<Temperature>);
        STATIC_REQUIRE(
            std::is_nothrow_move_constructible_v<Temperature>);
        STATIC_REQUIRE(
            std::is_nothrow_move_assignable_v<Temperature>);
    }

    SECTION("Temperature update")
    {
        using namespace angonoka::stun;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
        Temperature temp{
            Beta{1.f},
            BetaScale{1e-3f},
            StunWindow{5},
            RestartPeriod{64}};

        REQUIRE(temp == 1.f);

        for (int i{0}; i < 10; ++i) temp.update(1.f);

        REQUIRE(temp.average_stun() == Approx(1.));
        REQUIRE(temp > 1.f);

        for (int i{0}; i < 1000; ++i) temp.update(0.f);

        REQUIRE(temp.average_stun() == Approx(0.).margin(0.001));
        REQUIRE(temp < 1.f);
    }

    SECTION("Temperature special member functions")
    {
        using namespace angonoka::stun;

        Temperature temp{
            Beta{1.f},
            BetaScale{1e-3f},
            StunWindow{5},
            RestartPeriod{64}};

        SECTION("copy assignment")
        {
            Temperature temp2{
                Beta{2.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};

            temp = temp2;

            REQUIRE(temp == 2.f);
        }

        SECTION("move assignment")
        {
            Temperature temp2{
                Beta{2.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};

            temp = std::move(temp2);

            REQUIRE(temp == 2.f);
        }

        SECTION("copy ctor")
        {
            Temperature temp2{temp};

            REQUIRE(temp2 == 1.f);
        }

        SECTION("move ctor")
        {
            Temperature temp2{std::move(temp)};

            REQUIRE(temp2 == 1.f);
        }

        SECTION("self copy")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            temp = temp;
#pragma clang diagnostic pop

            REQUIRE(temp == 1.f);
        }

        SECTION("self move")
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            temp = std::move(temp);
#pragma clang diagnostic pop

            REQUIRE(temp == 1.f);
        }
    }
}

#pragma clang diagnostic pop
