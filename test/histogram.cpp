#include "histogram.h"
#include <catch2/catch.hpp>
#include <range/v3/numeric/accumulate.hpp>

TEST_CASE("histogram concepts")
{
    using namespace angonoka::detail;

    STATIC_REQUIRE(ranges::sized_range<Histogram>);
    STATIC_REQUIRE(ranges::random_access_range<Histogram>);

    Histogram hist{40};

    hist(1);
    hist(43);
    hist(44);
    hist(45);
    hist(45);
    hist(80);
    hist(80);

    REQUIRE(hist.size() == 3);

    auto it = hist.begin();
    {
        Bucket bucket = *it;

        REQUIRE(bucket.low == 0);
        REQUIRE(bucket.middle == 20);
        REQUIRE(bucket.high == 40);
        REQUIRE(static_cast<int32>(bucket) == 1);
        REQUIRE(bucket.count == 1);
    }

    ++it;

    {
        Bucket bucket = *it;

        REQUIRE(bucket.low == 40);
        REQUIRE(bucket.middle == 60);
        REQUIRE(bucket.high == 80);
        REQUIRE(static_cast<int32>(bucket) == 4);
        REQUIRE(bucket.count == 4);
    }

    // TODO: test implicit conversion to integral types

    REQUIRE(
        ranges::accumulate(hist, int32{0}, std::plus<int32>{}) == 7);
}
