#include "config/validation.h"
#include <boost/ut.hpp>
#include <yaml-cpp/yaml.h>

using namespace boost::ut;

suite validation = [] {
    "general validation"_test = [] {
        using namespace angonoka::validation;

        "basic map"_test = [] {
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
            expect(static_cast<bool>(schema(node)));
        };

        "implicit required"_test = [] {
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
            expect(static_cast<bool>(schema(node)));
        };

        "basic map with extra arg"_test = [] {
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
            expect(!result);
            expect(
                result.error()
                == R"(Unexpected attribute "val3" in "map")");
        };

        "empty attributes"_test = [] {
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
            expect(!result);
            expect(
                result.error() == R"(Empty attribute in "foobar")");
        };

        "map values"_test = [] {
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
            expect(static_cast<bool>(schema(node)));
        };
    };

    "any_of validation"_test = [] {
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

        "first variant"_test = [&] {
            // clang-format off
            const auto node = YAML::Load(
                "map:\n"
                "  val3:\n"
                "    val1: 1\n"
                "    val2: 2\n"
            );
            // clang-format on
            expect(static_cast<bool>(schema(node)));
        };

        "second variant"_test = [&] {
            // clang-format off
            const auto node = YAML::Load(
                "map:\n"
                "  val3: 3\n"
            );
            // clang-format on
            expect(static_cast<bool>(schema(node)));
        };

        "no attributes"_test = [&] {
            const auto node = YAML::Load("map:\n");
            const auto result = schema(node);
            expect(!result);
            expect(
                result.error()
                == R"("map" is missing a "val3" attribute)");
        };

        "extra args"_test = [&] {
            // clang-format off
            const auto node = YAML::Load(
                "map:\n"
                "  val3: 3\n"
                "  val2: 2\n"
            );
            // clang-format on
            const auto result = schema(node);
            expect(!result);
            expect(
                result.error()
                == R"(Unexpected attribute "val2" in "map")");
        };
    };
};
