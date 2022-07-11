#include <boost/container/flat_map.hpp>
#include <range/v3/range/concepts.hpp>
#include <concepts>
#include "common.h"

namespace angonoka::detail {

/**
    A histogram bin.

    @var count  Number of items in the bin
    @var low    Low threshold
    @var middle Middle value
    @var high   High threshold
*/
struct Bin {
    int32 count, low, middle, high;

    // TODO: not sure which functions are actually needed
    // constexpr operator int32() const noexcept { return count; }
    template<std::integral T>
    [[nodiscard]] constexpr operator T() const noexcept { return static_cast<T>(base_value(count)); }

    [[nodiscard]] constexpr auto operator<=>(const Bin& other) const noexcept {
        return base_value(count)<=>base_value(other.count);
    }

    template<std::integral T>
    [[nodiscard]] constexpr bool operator==(const T& other) const noexcept { return base_value(count) == other; }

    // TODO: ffs ranges thinks this isn't indirectly invokable with ranges::plus
    template<std::integral T>
    [[nodiscard]] friend constexpr auto operator+(const Bin&  bin, const T& i) noexcept { return base_value(bin.count) + i; }

    template<std::integral T>
    [[nodiscard]] friend constexpr auto operator+(const T& i, const Bin&  bin) noexcept { return bin+i; }
};

/**
    Histogram of integer values.
*/
class Histogram {
public:
    /**
        Bin iterator.
    */
    class Iterator;

    /**
        Constructor.

        @param bin_size Histogram bin size
    */
    explicit Histogram(int32 bin_size);

    /**
        Add a value to the histogram.

        @param value Value
    */
    void operator()(int32 value);

    /**
        Reset the histogram to an empty state
    */
    void clear();

    /**
        Number of non-empty bins in the histogram.
    */
    [[nodiscard]] std::size_t size() const;

    [[nodiscard]] bool empty() const;
    [[nodiscard]] Iterator begin() const noexcept;
    [[nodiscard]] Iterator end() const noexcept;
    Bin operator[](int32 index) const;

private:
    using Bins = boost::container::flat_map<int32, int32>;
    int32 bin_size;
    Bins bins;
};

class Histogram::Iterator {
public:
    using difference_type = Bins::const_iterator::difference_type;
    using value_type = Bin;

    Iterator(
        const Bins::const_iterator& iter,
        int32 bin_size) noexcept;
    Iterator() noexcept;

    Bin operator*() const;

    friend bool operator==(
        const Iterator& b,
        const Iterator& a) noexcept = default;
    Iterator& operator++() noexcept;
    Iterator operator++(int) noexcept;
    Iterator& operator--() noexcept;
    Iterator operator--(int) noexcept;
    friend difference_type
    operator-(const Iterator& a, const Iterator& b) noexcept;
    friend Iterator
    operator+(const Iterator& it, difference_type i) noexcept;
    friend Iterator
    operator+(difference_type i, const Iterator& it) noexcept;
    friend Iterator
    operator-(const Iterator& it, difference_type i) noexcept;
    friend Iterator
    operator-(difference_type i, const Iterator& it) noexcept;
    Iterator& operator+=(difference_type i) noexcept;
    Iterator& operator-=(difference_type i) noexcept;
    Bin operator[](difference_type i) const;
    std::strong_ordering
    operator<=>(const Iterator& other) const noexcept;

private:
    Bins::const_iterator iter;
    int32 bin_size;

    /**
        Make a bin from Bins key-value pair.

        @param v Bins key-value pair

        @return Histogram bin.
    */
    [[nodiscard]] Bin to_bin(Bins::const_reference v) const;
};
} // namespace angonoka::detail
