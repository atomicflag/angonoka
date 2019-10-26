#pragma once

#include <stdexcept>

namespace angonoka {
struct Exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
struct InvalidTasksDef : Exception {
	using Exception::Exception;
};
} // namespace angonoka
