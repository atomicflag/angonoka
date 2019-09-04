#pragma once

#include "../system.h"

namespace angonoka {
	/**
		Load System from a YAML string.

		@param text Null-terminated string

		@return An instance of System
	*/
	System load_text(const char* text);
} // namespace angonoka
