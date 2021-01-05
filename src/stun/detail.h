#pragma once

namespace angonoka::stun::detail {
/**
    Opaque floating point type.
*/
struct OpaqueFloat {
    float value;
    operator float() const noexcept { return value; }
};
} // namespace angonoka::stun::detail
