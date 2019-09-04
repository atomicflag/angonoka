#pragma once

#include <stdexcept>

namespace angonoka {
	struct InvalidTasksDefError : std::runtime_error {
		using std::runtime_error::runtime_error;
	};
} // namespace angonoka
