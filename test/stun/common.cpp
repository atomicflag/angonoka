#include "stun/common.h"
#include <catch2/catch.hpp>

"StateItem printing"_test = [] {
{
    using angonoka::stun::StateItem;

    const StateItem item{42, 123};

    expect(fmt::format("{}", item) == "(42, 123)");
}

"StateItem type traits"_test = [] {
{
    using angonoka::stun::StateItem;
    expect(std::is_nothrow_destructible_v<StateItem>);
    expect(std::is_default_constructible_v<StateItem>);
    expect(std::is_nothrow_copy_constructible_v<StateItem>);
    expect(std::is_nothrow_copy_assignable_v<StateItem>);
    expect(std::is_nothrow_move_constructible_v<StateItem>);
    expect(std::is_nothrow_move_assignable_v<StateItem>);
    expect(std::is_trivially_copyable_v<StateItem>);
}
