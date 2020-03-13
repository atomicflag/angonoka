#include <catch2/catch.hpp>
#include <type_traits>

#include "src/system.h"

TEST_CASE("System type traits")
{
    STATIC_REQUIRE(std::is_nothrow_destructible_v<angonoka::System>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<angonoka::System>);
    STATIC_REQUIRE(std::is_copy_constructible_v<angonoka::System>);
    STATIC_REQUIRE(std::is_copy_assignable_v<angonoka::System>);
    STATIC_REQUIRE(std::is_move_constructible_v<angonoka::System>);
    STATIC_REQUIRE(std::is_move_assignable_v<angonoka::System>);
}

TEST_CASE("Agent type traits")
{
    STATIC_REQUIRE(std::is_nothrow_destructible_v<angonoka::Agent>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<angonoka::Agent>);
    STATIC_REQUIRE(std::is_copy_constructible_v<angonoka::Agent>);
    STATIC_REQUIRE(std::is_copy_assignable_v<angonoka::Agent>);
    STATIC_REQUIRE(
        std::is_nothrow_move_constructible_v<angonoka::Agent>);
    STATIC_REQUIRE(
        std::is_nothrow_move_assignable_v<angonoka::Agent>);
}

TEST_CASE("Task type traits")
{
    STATIC_REQUIRE(std::is_nothrow_destructible_v<angonoka::Task>);
    STATIC_REQUIRE(
        std::is_nothrow_default_constructible_v<angonoka::Task>);
    STATIC_REQUIRE(std::is_copy_constructible_v<angonoka::Task>);
    STATIC_REQUIRE(std::is_copy_assignable_v<angonoka::Task>);
    STATIC_REQUIRE(
        std::is_nothrow_move_constructible_v<angonoka::Task>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<angonoka::Task>);
}
