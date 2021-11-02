#pragma once

namespace angonoka::stun {
struct Temperature {
    virtual operator float() noexcept = 0;
    virtual void update(float stun) noexcept = 0;
    [[nodiscard]] virtual float average_stun() const noexcept = 0;
    virtual ~Temperature() noexcept = default;
};
} // namespace angonoka::stun
