#include "stun/temperature.h"
#include <boost/ut.hpp>

using namespace boost::ut;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"

suite temperature = [] {
    "Temperature type traits"_test = [] {
        using angonoka::stun::Temperature;
        expect(std::is_nothrow_destructible_v<Temperature>);
        expect(!std::is_default_constructible_v<Temperature>);
        expect(std::is_copy_constructible_v<Temperature>);
        expect(std::is_copy_assignable_v<Temperature>);
        expect(std::is_nothrow_move_constructible_v<Temperature>);
        expect(std::is_nothrow_move_assignable_v<Temperature>);
    };

    "Temperature update"_test = [] {
        using namespace angonoka::stun;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
        Temperature temp{
            Beta{1.f},
            BetaScale{1e-3f},
            StunWindow{5},
            RestartPeriod{64}};

        expect(temp == 1.f);

        for (int i{0}; i < 10; ++i) temp.update(1.f);

        expect(temp.average_stun() == 1._d);
        expect(temp > 1.f);

        for (int i{0}; i < 1000; ++i) temp.update(0.f);

        expect(temp.average_stun() == 0._d);
        expect(temp < 1.f);
    };

    "Temperature special member functions"_test = [] {
        using namespace angonoka::stun;

        should("copy assignment") = [&] {
            Temperature temp{
                Beta{1.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};
            Temperature temp2{
                Beta{2.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};
            temp = temp2;
            expect(temp == 2.f);
        };

        should("move assignment") = [&] {
            Temperature temp{
                Beta{1.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};
            Temperature temp2{
                Beta{2.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};
            temp = std::move(temp2);
            expect(temp == 2.f);
        };

        should("copy ctor") = [&] {
            Temperature temp{
                Beta{1.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};
            Temperature temp2{temp};
            expect(temp2 == 1.f);
        };

        should("move ctor") = [&] {
            Temperature temp{
                Beta{1.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};
            Temperature temp2{std::move(temp)};
            expect(temp2 == 1.f);
        };

        should("self copy") = [&] {
            Temperature temp{
                Beta{1.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
            temp = temp;
#pragma clang diagnostic pop

            expect(temp == 1.f);
        };

        should("self move") = [&] {
            Temperature temp{
                Beta{1.f},
                BetaScale{1e-3f},
                StunWindow{5},
                RestartPeriod{64}};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
            temp = std::move(temp);
#pragma clang diagnostic pop

            expect(temp == 1.f);
        };
    };
};

#pragma clang diagnostic pop
