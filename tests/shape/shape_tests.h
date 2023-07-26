#pragma once
#include "../test_helpers.h"


namespace tests
{
    TEST_CASE("One dimensional shape")
    {
        auto shape = symd::Shape({3});

        REQUIRE(shape[0] == 3);
    }

    TEST_CASE("Shape 1")
    {
        auto shape = symd::Shape({3, 64, 128});

        REQUIRE(shape[0] == 3);
        REQUIRE(shape[1] == 64);
        REQUIRE(shape[2] == 128);
        REQUIRE(shape[-1] == 128);
        REQUIRE(shape[-2] == 64);
    }
}
