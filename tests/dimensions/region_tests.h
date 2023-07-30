#pragma once
#include "../../LibSymd/internal/region.h"


namespace tests
{
    TEST_CASE("Region shape test")
    {
        auto region = symd::__internal__::Region(symd::Dimensions({1, 33, 65}));
        auto shape = region.getShape();

        REQUIRE(shape.count() == 3);
        REQUIRE(shape[0] == 1);
        REQUIRE(shape[1] == 33);
        REQUIRE(shape[2] == 65);
    }

    TEST_CASE("Region align_with_symd_len test")
    {
        auto region = symd::__internal__::Region(symd::Dimensions({1, 33, 65}));
        auto aligned = region.align_with_symd_len(4);
        auto shape = aligned.getShape();

        REQUIRE(shape.count() == 3);
        REQUIRE(shape[0] == 1);
        REQUIRE(shape[1] == 33);
        REQUIRE(shape[2] == 64);
    }

    TEST_CASE("Region align_with_symd_len test 2")
    {
        auto startShape = symd::Dimensions({3, 4, 5});
        auto endShape = symd::Dimensions({3, 10, 33});

        auto region = symd::__internal__::Region(startShape, endShape);
        auto shape = region.getShape();

        REQUIRE(shape.count() == 3);
        REQUIRE(shape[0] == 1);
        REQUIRE(shape[1] == 7);
        REQUIRE(shape[2] == 29);

        auto aligned_region = region.align_with_symd_len(4);

        REQUIRE(aligned_region.startCoord.count() == 3);
        REQUIRE(aligned_region.startCoord[0] == 3);
        REQUIRE(aligned_region.startCoord[1] == 4);
        REQUIRE(aligned_region.startCoord[2] == 5);

        REQUIRE(aligned_region.endCoord.count() == 3);
        REQUIRE(aligned_region.endCoord[0] == 3);
        REQUIRE(aligned_region.endCoord[1] == 10);
        REQUIRE(aligned_region.endCoord[2] == 32);
    }
}
