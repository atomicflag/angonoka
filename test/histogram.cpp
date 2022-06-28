#include <catch2/catch.hpp>

#include <boost/container/flat_map.hpp>
#include <cstdint>
#include <boost/safe_numerics/safe_integer.hpp>
#include <range/v3/range/concepts.hpp>

namespace angonoka::detail {
    namespace sn = boost::safe_numerics;
    using int32 = sn::safe<std::int_fast32_t>;
    // TODO: doc, test, expects
    class Histogram {
        public:
            // TODO: doc, test, expects
            Histogram(int32 bucket_size) : bucket_size{bucket_size} {}

            // TODO: doc, test, expects
            void operator()(int32 value) {
                const int32 bucket = value/bucket_size;
                buckets[bucket] += value;
            }

            // TODO: doc, test, expects
            void clear() {
                buckets.clear();
            }

            // TODO: doc, test, expects
            auto begin() {
                // TODO: transform iterator
                return buckets.begin();
            }

            // TODO: doc, test, expects
            auto end() {
                // TODO: transform iterator
                return buckets.end();
            }
        private:
            int32 bucket_size;
            boost::container::flat_map<int32, int32> buckets;
    };
} // namespace angonoka::detail

TEST_CASE("histogram basic operations")
{
    using namespace angonoka::detail;

    STATIC_REQUIRE(ranges::sized_range<Histogram>);
    STATIC_REQUIRE(ranges::random_access_range<Histogram>);

    Histogram hist{50};

}
