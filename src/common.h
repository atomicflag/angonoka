#pragma once

#include <boost/container/flat_set.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/safe_numerics/automatic.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#include <cstdint>

namespace angonoka {
namespace sn = boost::safe_numerics;
using int8 = sn::safe<std::int_fast8_t, sn::automatic>;

template <typename T, auto N>
using Vector = boost::container::small_vector<T, N>;
template <typename T, auto N>
using Set = boost::container::flat_set<T, std::less<T>, Vector<T, N>>;
} // namespace angonoka
