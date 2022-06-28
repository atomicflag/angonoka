#include <catch2/catch.hpp>

#include <boost/container/flat_map.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#include <cstdint>
#include <range/v3/range/concepts.hpp>

namespace angonoka::detail {
namespace sn = boost::safe_numerics;
using int32 = sn::safe<std::int_fast32_t>;

// TODO: doc, test, expects
struct Bucket {
    int32 count, low, middle, high;

    operator int32() const { return count; }
};

using HistogramData = boost::container::flat_map<int32, int32>;

// TODO: doc, test, expects
struct BucketIter {
    Bucket operator()(HistogramData::reference v) const
    {
        return {
            .count{v.second},
            .low{v.first},
            .middle{v.first + bucket_size / 2},
            .high{v.first + bucket_size}};
    }

    int32 bucket_size;
};

class BucketIt : public boost::transform_iterator<BucketIter, HistogramData::iterator> {
    public:
        using transform_iterator::transform_iterator;
        using iterator_category = std::random_access_iterator_tag;
        // TODO: Wrong iterator category
        using transform_iterator::operator++;
};

// TODO: doc, test, expects
class Histogram {
public:
    // TODO: doc, test, expects
    Histogram(int32 bucket_size)
        : bucket_size{bucket_size}
    {
    }

    // TODO: doc, test, expects
    void operator()(int32 value)
    {
        const int32 bucket = value / bucket_size;
        buckets[bucket] += value;
    }

    // TODO: doc, test, expects
    void clear() { buckets.clear(); }

    // TODO: doc, test, expects
    BucketIt begin() noexcept
    {
        return BucketIt{            buckets.begin(),
            {bucket_size}};
    }

    // TODO: doc, test, expects
    BucketIt end() noexcept
    {
        return BucketIt{            buckets.end(),
            {bucket_size}};
    }

    auto size() const { return buckets.size(); }

private:
    int32 bucket_size;
    HistogramData buckets;
};
} // namespace angonoka::detail

namespace ranges {
    template<>
    constexpr bool enable_borrowed_range<angonoka::detail::Histogram> = true;
} // namespace ranges

TEST_CASE("histogram concepts")
{
    using namespace angonoka::detail;

    // std::iterator_traits<decltype(Histogram{0}.begin())>::iterator_category::foo = 42;


    Histogram hist{42};
    // ranges::begin(hist);
    STATIC_REQUIRE(ranges::detail::_borrowed_range<Histogram>);
    // STATIC_REQUIRE(ranges::input_or_output_iterator<decltype(hist.begin())>);
    // STATIC_REQUIRE(ranges::has_member_begin<Histogram>);
    // STATIC_REQUIRE(ranges::range<Histogram>);
    // STATIC_REQUIRE(ranges::sized_range<Histogram>);
    // STATIC_REQUIRE(ranges::random_access_range<Histogram>);
    STATIC_REQUIRE(requires(Histogram h) {
        {
            *h.begin()
            } -> std::same_as<Bucket>;
    });
    for(const auto& v : hist) {}
    hist.begin() == hist.end();
    auto i = hist.begin();
    STATIC_REQUIRE(std::same_as<decltype(i)&, decltype(++i)>);
}
