#pragma once

#include "../common.h"
#include "../concepts.h"
#include <boost/container/flat_set.hpp>
#include <boost/outcome/result.hpp>
#include <boost/outcome/try.hpp>
#include <fmt/format.h>
#include <gsl/gsl-lite.hpp>
#include <string>
#include <string_view>
#include <yaml-cpp/yaml.h>

namespace angonoka::validation {
using boost::container::flat_set;
namespace bo = boost::outcome_v2;
using result = bo::result<void, std::string>;
using namespace fmt::literals;

template <typename T>
concept Check
    = std::is_invocable_v<T, const YAML::Node&, gsl::czstring>;
template <typename T>
concept Attribute = std::is_convertible_v<
    decltype(std::declval<T>().name),
    std::string_view>&& Check<T>;
template <typename T> concept AttrOrStr = String<T> || Attribute<T>;

/**
    YAML scalar.

    Example:

    hello: "world"
           ^
           | Scalar

    scalar()

    Means that the value has to be a scalar and
    not a map or a sequence, etc.

    @return Check function
*/
consteval Check auto scalar()
{
    return [](const YAML::Node& node,
              std::string_view scope) -> result {
        if (!node || !node.IsScalar())
            return R"("{}" is expected to be a string)"_format(scope);
        return bo::success();
    };
}

namespace detail {
    /**
        Helper class for required and optional YAML fields.

        @var name   Parameter's name
        @var check  Function to apply to the field
    */
    template <Check T> struct functor {
        gsl::czstring name;
        T check;

        constexpr functor(gsl::czstring name, T check)
            : name{name}
            , check{check}
        {
        }
        explicit constexpr functor(gsl::czstring name)
            : functor{name, scalar()}
        {
        }
    };
} // namespace detail

/**
    Requred YAML field.

    Example:

    required("hello")

    Means that the field "hello" is required and
    it has to be a scalar value.

    @var name   Parameter's name
    @var check  Function to apply to the field
*/
template <Check T> struct required : detail::functor<T> {
    using detail::functor<T>::functor;
    result
    operator()(const YAML::Node& node, std::string_view scope) const
    {
        if (const auto n = node[this->name])
            return this->check(n, this->name);
        return R"("{}" is missing a "{}" attribute)"_format(
            scope,
            this->name);
    }
};

template <Check T> required(gsl::czstring, T) -> required<T>;
required(gsl::czstring)->required<decltype(scalar())>;

namespace detail {
    /**
        Extract the map attribute's name.

        If an attribute is a string literal, pass the argument as is.

        @param attr Either an attribute or a string literal

        @return Attribute name
    */
    constexpr auto attr_name(gsl::czstring attr) { return attr; }
    constexpr auto attr_name(Attribute auto&& attr)
    {
        return attr.name;
    }

    /**
        Extract or construct an attribute check function.

        If an attrubte is a string literal, construct
        the required attrubte with the string literal as it's
        name.

        @param attr Either an attribute or a string literal

        @return Check function
    */
    constexpr auto attr_check(gsl::czstring attr)
    {
        return required(attr);
    }
    constexpr auto attr_check(Attribute auto&& attr) { return attr; }
} // namespace detail

/**
    Optional YAML field.

    Example:

    optional("hello")

    Means that the field "hello" is optional and if
    present, it has to be a scalar value.

    @var name  Parameter's name
    @var check Function to apply to the field
*/
template <Check T> struct optional : detail::functor<T> {
    using detail::functor<T>::functor;

    result operator()(
        const YAML::Node& node,
        std::string_view /* scope */) const
    {
        if (const auto n = node[this->name])
            return this->check(n, this->name);
        return bo::success();
    }
};

template <Check T> optional(gsl::czstring, T) -> optional<T>;
optional(gsl::czstring)->optional<decltype(scalar())>;

/**
    YAML array.

    Validates each value of the array with the provided function.

    Example:

    sequence(scalar())

    Means that the value has to be a sequence (array) of
    scalar values.

    @param check Function to apply to each item

    @return Check function
*/
consteval Check auto sequence(Check auto check)
{
    return [=](const YAML::Node& node,
               std::string_view scope) -> result {
        if (!node || !node.IsSequence()) {
            return R"("{}" is expected to be a sequence)"_format(
                node.Scalar());
        }
        const auto s = "Element of {}"_format(scope);
        for (auto&& a : node) { BOOST_OUTCOME_TRY(check(a, s)); }
        return bo::success();
    };
}

consteval Check auto sequence() { return sequence(scalar()); }

/**
    YAML map.

    Matches specified parameters exactly, no extra fields permitted.

    Example:

    attributes("first", optional("second"))

    Means that the value has to be a map with a required field
   "first", which has to be a scalar and an optional field "second"
   which also has to be a scalar.

    @param attrs Sequence of optional or required parameters

    @return Check function
*/
consteval Check auto attributes(AttrOrStr auto... attrs)
{
    return [=](const YAML::Node& node,
               std::string_view scope = "Document") -> result {
        if (!node || node.IsScalar() || node.IsSequence())
            return R"("{}" is expected to be a map)"_format(scope);
        flat_set<std::string_view> unique_fields;
        for (auto&& n : node) {
            const auto& attr_name = n.first.Scalar();
            if (attr_name.empty())
                return R"(Empty attribute in "{}")"_format(scope);
            if (!unique_fields.emplace(attr_name).second) {
                return R"(Duplicate attribute "{}" in "{}")"_format(
                    attr_name,
                    scope);
            }
            if (!((attr_name == detail::attr_name(attrs)) || ...)) {
                return R"(Unexpected attribute "{}" in "{}")"_format(
                    attr_name,
                    scope);
            }
        }
        result r = bo::success();
        ((r = detail::attr_check(attrs)(node, scope)) && ...);
        return r;
    };
}

/**
    YAML map.

    Validates each value of the map with the provided function.
    Used when the number of map fields may vary.

    Example:

    foo:
      bar1: 1
      bar2: 2
      bar3: 3

    values(scalar())

    @param check Function to apply to each value

    @return Check function
*/
consteval Check auto values(Check auto check)
{
    return [=](const YAML::Node& node,
               std::string_view scope) -> result {
        if (!node || !node.IsMap())
            return R"("{}" is expected to be a map)"_format(scope);
        for (auto&& n : node) {
            BOOST_OUTCOME_TRY(check(n.second, n.first.Scalar()));
        }
        return bo::success();
    };
}

/**
    Match at least one of the validators.

    Example:

    required("example", any_of(scalar(), attributes("foo", "bar")))

    Means the value has to either be a singular scalar value or
    a map with 2 fields "foo" and "bar.

    example: "hello"

    or

    example:
      foo: 1
      bar: 2

    @param check Functions to match

    @return Check function
*/
consteval Check auto any_of(Check auto... checks)
{
    return [=](const YAML::Node& node,
               std::string_view scope) -> result {
        result r = bo::success();
        ((r = checks(node, scope)) || ...);
        return r;
    };
}

} // namespace angonoka::validation
