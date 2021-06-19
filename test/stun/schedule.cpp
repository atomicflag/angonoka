#include "stun/schedule.h"
#include <boost/ut.hpp>

using namespace boost::ut;

suite stun_common = [] {
    "ScheduleItem printing"_test = [] {
        using angonoka::stun::ScheduleItem;

        const ScheduleItem item{42, 123};

        expect(fmt::format("{}", item) == "(42, 123)");
    };

    "ScheduleItem type traits"_test = [] {
        using angonoka::stun::ScheduleItem;
        expect(std::is_nothrow_destructible_v<ScheduleItem>);
        expect(std::is_default_constructible_v<ScheduleItem>);
        expect(std::is_nothrow_copy_constructible_v<ScheduleItem>);
        expect(std::is_nothrow_copy_assignable_v<ScheduleItem>);
        expect(std::is_nothrow_move_constructible_v<ScheduleItem>);
        expect(std::is_nothrow_move_assignable_v<ScheduleItem>);
        expect(std::is_trivially_copyable_v<ScheduleItem>);
    };
};
