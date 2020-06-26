#pragma once

#include "../common.h"
#include "../concepts.h"
#include <boost/outcome/result.hpp>
#include <boost/outcome/try.hpp>
#include <fmt/format.h>
#include <gsl/gsl-lite.hpp>
#include <string>
#include <string_view>
#include <yaml-cpp/yaml.h>

namespace angonoka::validation {

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
*/
consteval auto scalar()
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

        consteval functor(gsl::czstring name, T check)
            : name{name}
            , check{check}
        {
        }
        explicit consteval functor(gsl::czstring name)
            : functor{name, scalar()}
        {
        }
    };
} // namespace detail

/**
    Requred YAML field.

    @var name   Parameter's name
    @var check  Function to apply to the field
*/
template <Check T> struct required : detail::functor<T> {
    using detail::functor<T>::functor;
    result
    operator()(const YAML::Node& node, std::string_view scope) const
    {
        const auto n = node[this->name];
        if (!n) {
            return R"("{}" is missing a "{}" attribute)"_format(
                scope,
                this->name);
        }
        return this->check(n, this->name);
    }
};

template <Check T> required(gsl::czstring, T) -> required<T>;
required(gsl::czstring)->required<decltype(scalar())>;

namespace detail {
    /**
        Extract the map attribute's name.

        If an attribute is a string literal, pass the argument as is.

        @param attr Either an attribute or a string literal
    */
    consteval auto attr_name(gsl::czstring attr) { return attr; }
    consteval auto attr_name(Attribute auto&& attr)
    {
        return attr.name;
    }

    /**
        Extract or construct an attribute check function.

        If an attrubte is a string literal, construct
        the required attrubte with the string literal as it's
        name.

        @param attr Either an attribute or a string literal
    */
    consteval auto attr_check(gsl::czstring attr)
    {
        return required(attr);
    }
    consteval auto attr_check(Attribute auto&& attr) { return attr; }
} // namespace detail

/**
    Optional YAML field.

    @var name  Parameter's name
    @var check Function to apply to the field
*/
template <Check T> struct optional : detail::functor<T> {
    using detail::functor<T>::functor;

    result operator()(
        const YAML::Node& node,
        std::string_view /* scope */) const
    {
        if (const auto n = node[this->name]; n)
            return this->check(n, this->name);
        return bo::success();
    }
};

template <Check T> optional(gsl::czstring, T) -> optional<T>;
optional(gsl::czstring)->optional<decltype(scalar())>;

/**
    YAML array.

    Validates each value of the array with the provided function.

    @param check Function to apply to each item
*/
consteval auto sequence(Check auto check)
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

consteval auto sequence() { return sequence(scalar()); }

/**
    YAML map.

    Matches specified parameters exactly, no extra fields permitted.

    @param attrs Sequence of optional or required parameters
*/
consteval auto attributes(AttrOrStr auto... attrs)
{
    return [=](const YAML::Node& node,
               std::string_view scope = "Document") -> result {
        if (!node || node.IsScalar() || node.IsSequence())
            return R"("{}" is expected to be a map)"_format(scope);
        constexpr auto static_alloc_size = sizeof...(attrs);
        Set<std::string_view, static_alloc_size> unique_fields;
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

    For example:

    foo:
      bar1: 1
      bar2: 2
      bar3: 3

    values(scalar())

    @param check Function to apply to each value
*/
consteval auto values(Check auto check)
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

    @param check Functions to match
*/
consteval auto any_of(Check auto... checks)
{
    return [=](const YAML::Node& node,
               std::string_view scope) -> result {
        result r = bo::success();
        ((r = checks(node, scope)) || ...);
        return r;
    };
}

} // namespace angonoka::validation
