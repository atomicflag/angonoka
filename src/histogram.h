#include <boost/container/flat_map.hpp>
#include <range/v3/range/concepts.hpp>
#include "common.h"

namespace angonoka::detail {

/**
    A histogram bucket.

    @var count  Number of items in the bucket
    @var low    Low threshold
    @var middle Middle value
    @var high   High threshold
*/
struct Bucket {
    int32 count, low, middle, high;

    operator int32() const noexcept { return count; }
    auto operator<=>(const Bucket& other) const noexcept {
        return count<=>other.count;
    }
};

/**
    Histogram of integer values.
*/
class Histogram {
public:
    /**
        Bucket iterator.
    */
    class Iterator;

    /**
        Constructor.

        @param bucket_size Histogram bucket size
    */
    explicit Histogram(int32 bucket_size);

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
        Number of non-empty buckets in the histogram.
    */
    [[nodiscard]] std::size_t size() const;

    [[nodiscard]] bool empty() const;
    [[nodiscard]] Iterator begin() const noexcept;
    [[nodiscard]] Iterator end() const noexcept;
    Bucket operator[](int32 index) const;

private:
    using Buckets = boost::container::flat_map<int32, int32>;
    int32 bucket_size;
    Buckets buckets;
};

class Histogram::Iterator {
public:
    using difference_type = Buckets::const_iterator::difference_type;
    using value_type = Bucket;

    Iterator(
        const Buckets::const_iterator& iter,
        int32 bucket_size) noexcept;
    Iterator() noexcept;

    Bucket operator*() const;

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
    Bucket operator[](difference_type i) const;
    std::strong_ordering
    operator<=>(const Iterator& other) const noexcept;

private:
    Buckets::const_iterator iter;
    int32 bucket_size;

    /**
        Make a bucket from Buckets key-value pair.

        @param v Buckets key-value pair

        @return Histogram bucket.
    */
    [[nodiscard]] Bucket to_bucket(Buckets::const_reference v) const;
};
} // namespace angonoka::detail
