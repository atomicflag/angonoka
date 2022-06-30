#include <catch2/catch.hpp>

#include <boost/container/flat_map.hpp>
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


// TODO: doc, test, expects
class Histogram {
public:
    using Data = boost::container::flat_map<int32, int32>;
    // TODO: doc, test, expects
    class Iterator {
        public:
            using difference_type = Data::const_iterator::difference_type;
            using value_type = Bucket;

            // TODO: doc, test, expects
            Iterator(Data::const_iterator iter, int32 bucket_size) noexcept : iter{iter}, bucket_size{bucket_size} {}
            Iterator() noexcept = default;

            Bucket operator*() const {
                return to_bucket(*iter);
            }

            friend bool operator==(const Iterator& b, const Iterator& a) noexcept {
                return a.iter == b.iter;
            }

            Iterator& operator++() noexcept {
                ++iter;
                return *this;
            }

            Iterator operator++(int) noexcept {
                return {iter++, bucket_size};
            }

            Iterator& operator--() noexcept {
                --iter;
                return *this;
            }

            Iterator operator--(int) noexcept {
                return {iter--, bucket_size};
            }

            friend difference_type operator-(const Iterator& a, const Iterator& b) noexcept {
                return a.iter - b.iter;
            }

            friend Iterator operator+(const Iterator& it, difference_type i) noexcept {
                return {it.iter+i, it.bucket_size};
            }

            friend Iterator operator+(difference_type i, const Iterator& it) noexcept {
                return {it.iter+i, it.bucket_size};
            }

            friend Iterator operator-(const Iterator& it, difference_type i) noexcept {
                return {it.iter-i, it.bucket_size};
            }

            friend Iterator operator-(difference_type i, const Iterator& it) noexcept {
                return {it.iter-i, it.bucket_size};
            }

            Iterator& operator+=(difference_type i) noexcept {
                iter += i;
                return *this;
            }

            Iterator& operator-=(difference_type i) noexcept {
                iter -= i;
                return *this;
            }

            Bucket operator[](difference_type i) const noexcept {
                return to_bucket(iter[i]);
            }

            std::strong_ordering operator<=>(const Iterator& other) const noexcept {
                if(iter == other.iter) return std::strong_ordering::equal;
                if(iter < other.iter) return std::strong_ordering::less;
                return std::strong_ordering::greater;
            }

        private:
                Data::const_iterator iter;
                int32 bucket_size;

                Bucket to_bucket(const auto& v) const {
            return {
                .count{v.second},
                .low{v.first},
                .middle{v.first + bucket_size / 2},
                .high{v.first + bucket_size}};
                }

    };

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
    Iterator begin() const noexcept
    {
        return Iterator{buckets.begin(), bucket_size};
    }

    // TODO: doc, test, expects
    Iterator end() const noexcept
    {
        return Iterator{buckets.end(), bucket_size};
    }

    auto operator[](int32 index) const {
        return *Iterator{buckets.find(index), bucket_size};
    }

    auto size() const { return buckets.size(); }

private:
    int32 bucket_size;
    Data buckets;
};
} // namespace angonoka::detail


TEST_CASE("histogram concepts")
{
    using namespace angonoka::detail;

    // std::iterator_traits<decltype(Histogram{0}.begin())>::iterator_category::foo = 42;


    Histogram hist{42};
    ranges::begin(hist);
    STATIC_REQUIRE(ranges::sized_range<Histogram>);
    STATIC_REQUIRE(ranges::random_access_range<Histogram>);
    ranges::end(hist);
    STATIC_REQUIRE(requires(Histogram h) {
        {
            *h.begin()
            } -> std::same_as<Bucket>;
    });
    // TODO: fix tests
    for(const auto& v : hist) {}
    hist.begin() == hist.end();
    auto i = hist.begin();
    i = hist.end();
    // ranges::iter_reference_t<decltype(i)>::foo = 1; // >:(
    // decltype(i[5])::foo = 1;
    // STATIC_REQUIRE(std::same_as<decltype(i)&, decltype(++i)>);
}
