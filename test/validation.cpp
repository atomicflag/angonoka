#include "config/validation.h"
#include <catch2/catch.hpp>
#include <yaml-cpp/yaml.h>

TEST_CASE("validation")
{
    using namespace angonoka::validation;

    SECTION("general validation")
    {
        SECTION("basic map")
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

        SECTION("implicit required")
        {
            // clang-format off
            constexpr auto schema = attributes(
                required("map", attributes(
                    "val1",
                    "val2",
                    optional("val3")
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

            REQUIRE(schema(node));
        }

        SECTION("basic map with extra arg")
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
                == R"(Unexpected attribute "val3" in "map".)");
        }

        SECTION("empty attributes")
        {
            // clang-format off
            constexpr auto schema = attributes(
                required("foobar", attributes(
                    required("val1")
                ))
            );
            // clang-format on

            // clang-format off
            const auto node = YAML::Load(
                "foobar:\n"
                "  val1: 3\n"
                "  : 3\n"
            );
            // clang-format on

            const auto result = schema(node);

            REQUIRE_FALSE(result);
            REQUIRE(
                result.error() == R"(Empty attribute in "foobar".)");
        }

        SECTION("map values")
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
        };
    }

    SECTION("any_of validation")
    {
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

        SECTION("first variant")
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

        SECTION("second variant")
        {
            // clang-format off
            const auto node = YAML::Load(
                "map:\n"
                "  val3: 3\n"
            );
            // clang-format on

            REQUIRE(schema(node));
        };

        SECTION("no attributes")
        {
            const auto node = YAML::Load("map:\n");

            const auto result = schema(node);

            REQUIRE_FALSE(result);
            REQUIRE(
                result.error()
                == R"("map" is missing a "val3" attribute.)");
        }

        SECTION("extra args")
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
                == R"(Unexpected attribute "val2" in "map".)");
        }
    }

    SECTION("nested location")
    {
        // clang-format off
        constexpr auto schema = attributes(
            required("first", attributes(
                required("second", attributes(
                    required("third")
                ))
            ))
        );
        // clang-format on

        SECTION("deep nesting")
        {
            // clang-format off
            const auto node = YAML::Load(
                "first:\n"
                "  second:\n"
                "    third:\n"
                "      error: 123\n"
            );
            // clang-format on

            const auto result = schema(node);

            REQUIRE_FALSE(result);
            REQUIRE(
                result.error()
                == R"("first.second.third" has invalid type.)");
        }

        SECTION("shallow")
        {
            // clang-format off
            const auto node = YAML::Load(
                "first:\n"
                "  a: 123\n"
            );
            // clang-format on

            const auto result = schema(node);

            REQUIRE_FALSE(result);
            REQUIRE(
                result.error()
                == R"(Unexpected attribute "a" in "first".)");
        }
    }

    SECTION("invalid attributes")
    {
        // clang-format off
        constexpr auto schema = attributes(
            required("attr")
        );
        // clang-format on

        SECTION("invalid type")
        {
            // clang-format off
            const auto node = YAML::Load(
                "attr: []\n"
            );
            // clang-format on

            const auto result = schema(node);

            REQUIRE_FALSE(result);
            REQUIRE(result.error() == R"("attr" has invalid type.)");
        }

        SECTION("empty")
        {
            // clang-format off
            const auto node = YAML::Load(
                "attr:\n"
            );
            // clang-format on

            const auto result = schema(node);

            REQUIRE_FALSE(result);
            REQUIRE(result.error() == R"("attr" can't be empty.)");
        }

        SECTION("empty attribute name")
        {
            // clang-format off
            const auto node = YAML::Load(
                ": hello\n"
            );
            // clang-format on

            const auto result = schema(node);

            REQUIRE_FALSE(result);
            REQUIRE(result.error() == R"(Empty attribute in "".)");
        }
    }
}
