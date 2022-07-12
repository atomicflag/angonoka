#include "histogram.h"
#include <catch2/catch.hpp>
#include <range/v3/numeric/accumulate.hpp>

TEST_CASE("histogram concepts")
{
    using namespace angonoka::detail;
    using angonoka::int32;

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
        Bin bin = *it;

        REQUIRE(bin.low == 0);
        REQUIRE(bin.middle == 20);
        REQUIRE(bin.high == 40);
        REQUIRE(bin == 1);
        REQUIRE(static_cast<int>(bin) == 1);
        REQUIRE(bin.count == 1);
    }

    ++it;

    {
        Bin bin = *it;

        REQUIRE(bin.low == 40);
        REQUIRE(bin.middle == 60);
        REQUIRE(bin.high == 80);
        REQUIRE(static_cast<int>(bin) == 4);
        REQUIRE(bin.count == 4);
    }

    REQUIRE(ranges::accumulate(hist, 0.F) == 7.F);
}
