#pragma once

#include "../exceptions.h"
#include <fmt/format.h>
#include <range/v3/algorithm/count_if.hpp>
#include <string_view>
#include <yaml-cpp/yaml.h>

namespace angonoka::validation {
using ranges::count_if;

/**
  Helper class for required YAML parameters.
*/
template <typename T> struct Required {
    const char* name;
    T check;

    void
    operator()(const YAML::Node& node, std::string_view scope) const
    {
        // False positive
        const auto n = node[name]; // NOLINT
        if (!n) {
            constexpr auto err_text
                = R"_("{}" is missing a "{}" attribute)_";
            throw InvalidTasksDef{fmt::format(err_text, scope, name)};
        }
        auto same_name
            = [&](auto&& a) { return a.first.Scalar() == name; };
        if (count_if(node, same_name) > 1) {
            constexpr auto err_text
                = R"_("{}" has duplicate "{}" attributes)_";
            throw InvalidTasksDef{fmt::format(err_text, scope, name)};
        }
        check(n, name);
    }
};

/**
  Requred YAML parameter.

  @param name   Parameter's name
  @param check  Function to apply to the parameter

  @return A function-like object of type Required
*/
template <typename T>
constexpr auto required(const char* name, T check)
{
    return Required<T>{name, check};
}

/**
  Helper class for optional YAML parameters.
*/
template <typename T> struct Optional {
    const char* name;
    T check;

    void operator()(
        const YAML::Node& node,
        std::string_view /* scope */) const
    {
        // False positive
        const auto n = node[name]; // NOLINT
        if (n) check(n, name);
    }
};

/**
  Optional YAML parameter.

  @param name  Parameter's name
  @param check Function to apply to the parameter
*/
template <typename T>
constexpr auto optional(const char* name, T check)
{
    return Optional<T>{name, check};
}

/**
  YAML array.

  @param check Function to apply to each item
*/
template <typename T> constexpr auto sequence(T check)
{
    return [=](const YAML::Node& node, std::string_view scope) {
        if (!node || !node.IsSequence()) {
            constexpr auto err_text
                = R"_("{}" is expected to be a sequence)_";
            throw InvalidTasksDef{err_text};
        }
        const auto s = fmt::format("Element of {}", scope);
        for (auto&& a : node) check(a, s);
    };
}

/**
  YAML scalar.
*/
constexpr auto scalar()
{
    return [](const YAML::Node& node, std::string_view scope) {
        if (!node || !node.IsScalar()) {
            constexpr auto err_text
                = R"_("{}" is expected to be a string)_";
            throw InvalidTasksDef{fmt::format(err_text, scope)};
        }
    };
}

/**
  YAML map.

  Checks parameters and throws on unexpected attributes.

  @param attrs Sequence of optional or required parameters
*/
template <typename... T> constexpr auto attributes(T... attrs)
{
    return [=](const YAML::Node& node,
               std::string_view scope = "Document") {
        if (!node || node.IsScalar() || node.IsSequence()) {
            constexpr auto err_text
                = R"_("{}" is expected to be a map)_";
            throw InvalidTasksDef{fmt::format(err_text, scope)};
        }
        for (auto&& n : node) {
            const auto attr_name = n.first.Scalar();
            if (!((attr_name == attrs.name) || ...)) {
                constexpr auto err_text
                    = R"_(Unexpected attribute "{}" in "{}")_";
                throw InvalidTasksDef{
                    fmt::format(err_text, attr_name, scope)};
            }
        }
        (attrs(node, scope), ...);
    };
}

/**
  YAML map.

  @param check Function to apply to each value
*/
template <typename T> constexpr auto map(T check)
{
    return [=](const YAML::Node& node, std::string_view scope) {
        if (!node || !node.IsMap()) {
            constexpr auto err_text
                = R"_("{}" is expected to be a map)_";
            throw InvalidTasksDef{fmt::format(err_text, scope)};
        }
        for (auto&& n : node) check(n.second, n.first.Scalar());
    };
}

} // namespace angonoka::validation
