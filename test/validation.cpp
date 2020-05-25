#include <catch2/catch.hpp>

#include "config/validation.h"
#include <yaml-cpp/yaml.h>

TEST_CASE("General validation")
{
    using namespace angonoka::validation;

    SECTION("Basic map")
    {
        // clang-format off
        constexpr auto schema = attributes(
            required("map", attributes(
                required("val1"),
                required("val2")
            ))
        );
        // clang-format on

        // clang-format off
        const auto node = YAML::Load(
            "map:\n"
            "  val1: 1\n"
            "  val2: 2\n"
        );
        // clang-format on
        REQUIRE(schema(node));
    }

    SECTION("Basic map with extra arg")
    {
        // clang-format off
        constexpr auto schema = attributes(
            required("map", attributes(
                required("val1"),
                required("val2")
            ))
        );
        // clang-format on

        // clang-format off
        const auto node = YAML::Load(
            "map:\n"
            "  val1: 1\n"
            "  val2: 2\n"
            "  val3: 3\n"
        );
        // clang-format on
        const auto result = schema(node);
        REQUIRE_FALSE(result);
        REQUIRE(
            result.error()
            == R"(Unexpected attribute "val3" in "map")");
    }

    SECTION("Map values")
    {
        // clang-format off
        constexpr auto schema = attributes(
            required("map",
                values(scalar())
            )
        );
        // clang-format on

        // clang-format off
        const auto node = YAML::Load(
            "map:\n"
            "  val1: 1\n"
            "  val2: 2\n"
            "  val3: 3\n"
        );
        // clang-format on
        REQUIRE(schema(node));
    }
}

TEST_CASE("any_of validation")
{
    using namespace angonoka::validation;

    // clang-format off
    constexpr auto schema = attributes(
        required("map", any_of(
            attributes(
                required("val3", attributes(
                    required("val1"),
                    required("val2")
                ))
            ),
            attributes(
                required("val3")
            )
        ))
    );
    // clang-format on

    SECTION("First variant")
    {
        // clang-format off
        const auto node = YAML::Load(
            "map:\n"
            "  val3:\n"
            "    val1: 1\n"
            "    val2: 2\n"
        );
        // clang-format on
        REQUIRE(schema(node));
    }

    SECTION("Second variant")
    {
        // clang-format off
        const auto node = YAML::Load(
            "map:\n"
            "  val3: 3\n"
        );
        // clang-format on
        REQUIRE(schema(node));
    }

    SECTION("No attributes")
    {
        const auto node = YAML::Load("map:\n");
        const auto result = schema(node);
        REQUIRE_FALSE(result);
        REQUIRE(
            result.error()
            == R"("map" is missing a "val3" attribute)");
    }

    SECTION("Extra args")
    {
        // clang-format off
        const auto node = YAML::Load(
            "map:\n"
            "  val3: 3\n"
            "  val2: 2\n"
        );
        // clang-format on
        const auto result = schema(node);
        REQUIRE_FALSE(result);
        REQUIRE(
            result.error()
            == R"(Unexpected attribute "val2" in "map")");
    }
}
