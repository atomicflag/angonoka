#pragma once

#include <boost/safe_numerics/safe_integer.hpp>
#include <cstdint>

namespace angonoka {
namespace sn = boost::safe_numerics;
using int8 = sn::safe<std::int_fast8_t>;
} // namespace angonoka
