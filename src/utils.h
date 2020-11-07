#include <array>
#include <type_traits>

namespace angonoka::utils {
/**
    Constructs an std::array from arguments

    This function is more convinient to use than std::array
    deduction guides in certain situations.

    @param values Arguments

    @return Array
*/
template <typename T = void>
consteval auto make_array(auto... values) noexcept
{
    using Type = std::conditional_t<
        std::is_same_v<T, void>,
        std::common_type_t<decltype(values)...>,
        T>;
    static_assert(
        (std::is_convertible_v<Type, decltype(values)> && ...),
        "All arguments must be convertable to Type");
    return std::array<Type, sizeof...(values)>{
        static_cast<Type>(values)...};
}
} // namespace angonoka::utils
