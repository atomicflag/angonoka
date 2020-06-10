#pragma once

#include <boost/container/flat_set.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/safe_numerics/automatic.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/safe_numerics/safe_integer_range.hpp>

namespace angonoka {
namespace sn = boost::safe_numerics;
using Integer = sn::safe<int, sn::automatic>;
template <auto L, auto U>
using Constrained = sn::safe_signed_range<L, U, sn::automatic>;

template <typename T, auto N>
using Vector = boost::container::small_vector<T, N>;
template <typename T, auto N>
using Set = boost::container::flat_set<T, std::less<T>, Vector<T, N>>;
} // namespace angonoka
