#pragma once

#include <fmt/format.h>
#include <string_view>
#include <yaml-cpp/yaml.h>

#include "errors.h"

namespace angonoka::validation {
template <typename T> struct Required {
	const char* name;
	T check;

	constexpr void operator()(
		const YAML::Node& node, std::string_view scope) const
	{
		const auto n = node[name];
		if (!n) {
			constexpr auto err_text
				= "\"{}\" is missing a \"{}\" attribute";
			throw InvalidTasksDefError{
				fmt::format(err_text, scope, name)};
		}
		check(n, name);
	}
};

template <typename T>
constexpr auto required(const char* name, T check)
{
	return Required<T>{name, check};
}

template <typename T> struct Optional {
	const char* name;
	T check;

	constexpr void operator()(
		const YAML::Node& node, std::string_view) const
	{
		const auto n = node[name];
		if (n) check(n, name);
	}
};

template <typename T>
constexpr auto optional(const char* name, T check)
{
	return Optional<T>{name, check};
}

template <typename T> constexpr auto sequence(T check)
{
	return [=](const YAML::Node& node, std::string_view scope) {
		if (!node || !node.IsSequence()) {
			constexpr auto err_text
				= "\"{}\" is expected to be a sequence";
			throw InvalidTasksDefError{err_text};
		}
		const auto s = fmt::format("Element of {}", scope);
		for (auto&& a : node) check(a, s);
	};
}

constexpr auto scalar()
{
	return [](const YAML::Node& node, std::string_view scope) {
		if (!node || !node.IsScalar()) {
			constexpr auto err_text
				= "\"{}\" is expected to be a string";
			throw InvalidTasksDefError{fmt::format(err_text, scope)};
		}
	};
}

template <typename... T> constexpr auto attributes(T... attrs)
{
	return [=](const YAML::Node& node,
			   std::string_view scope = "Document") {
		if (!node || node.IsScalar() || node.IsSequence()) {
			constexpr auto err_text
				= "\"{}\" is expected to be a map";
			throw InvalidTasksDefError{fmt::format(err_text, scope)};
		}
		for (auto&& n : node) {
			const auto attr_name = n.first.Scalar();
			if (!((attr_name == attrs.name) || ...)) {
				constexpr auto err_text
					= "Unexpected attribute \"{}\" in \"{}\"";
				throw InvalidTasksDefError{
					fmt::format(err_text, attr_name, scope)};
			}
		}
		(attrs(node, scope), ...);
	};
}

template <typename T> constexpr auto map(T check)
{
	return [=](const YAML::Node& node, std::string_view scope) {
		if (!node || !node.IsMap()) {
			constexpr auto err_text
				= "\"{}\" is expected to be a map";
			throw InvalidTasksDefError{fmt::format(err_text, scope)};
		}
		for (auto&& n : node) check(n.second, n.first.Scalar());
	};
}

} // namespace angonoka::validation
