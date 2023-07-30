#pragma once
#include "../../LibSymd/dimensions.h"


namespace tests
{
    TEST_CASE("One dimensional shape")
    {
        auto shape = symd::Dimensions({3});

        REQUIRE(shape[0] == 3);
    }

    TEST_CASE("Shape 1")
    {
        auto shape = symd::Dimensions({3, 64, 128});

        REQUIRE(shape[0] == 3);
        REQUIRE(shape[1] == 64);
        REQUIRE(shape[2] == 128);
        REQUIRE(shape[-1] == 128);
        REQUIRE(shape[-2] == 64);
    }

    TEST_CASE("Dimensions: test native pitch")
    {
        auto shape = symd::Dimensions({3, 64, 128});
        auto pitch = shape.native_pitch();

        REQUIRE(pitch.count() == 3);
        REQUIRE(pitch[2] == 1);
        REQUIRE(pitch[1] == 128);
        REQUIRE(pitch[0] == 128*64);
    }

    TEST_CASE("Dimensions: zeros_like test")
    {
        auto shape = symd::Dimensions({3, 64, 128});
        auto zeros_like = shape.zeros_like();

        REQUIRE(zeros_like.count() == 3);
        REQUIRE(zeros_like[2] == 0);
        REQUIRE(zeros_like[1] == 0);
        REQUIRE(zeros_like[0] == 0);
    }

    TEST_CASE("Dimensions: native_border test")
    {
        auto shape = symd::Dimensions({3, 64, 128});
        auto native_border = shape.native_border();

        REQUIRE(native_border.count() == 3);
        REQUIRE(native_border[2] == 0);
        REQUIRE(native_border[1] == 0);
        REQUIRE(native_border[0] == 0);
    }

    TEST_CASE("Dimensions: operator+ other Dimensions")
    {
        auto a = symd::Dimensions({3, 64, 128});
        auto b = symd::Dimensions({1, 2, 3});
        auto res = a + b;

        REQUIRE(res.count() == 3);
        REQUIRE(res[0] == 4);
        REQUIRE(res[1] == 66);
        REQUIRE(res[2] == 131);
    }

    TEST_CASE("Dimensions: operator- other Dimensions")
    {
        auto a = symd::Dimensions({3, 64, 128});
        auto b = symd::Dimensions({1, 2, 3});
        auto res = a - b;

        REQUIRE(res.count() == 3);
        REQUIRE(res[0] == 2);
        REQUIRE(res[1] == 62);
        REQUIRE(res[2] == 125);
    }

    TEST_CASE("Dimensions: operator+ other int")
    {
        auto a = symd::Dimensions({3, 64, 128});
        auto res = a + 1;

        REQUIRE(res.count() == 3);
        REQUIRE(res[0] == 4);
        REQUIRE(res[1] == 65);
        REQUIRE(res[2] == 129);
    }

    TEST_CASE("Dimensions: operator- other int")
    {
        auto a = symd::Dimensions({3, 64, 128});
        auto res = a - 1;

        REQUIRE(res.count() == 3);
        REQUIRE(res[0] == 2);
        REQUIRE(res[1] == 63);
        REQUIRE(res[2] == 127);
    }

    TEST_CASE("Dimensions: num_elements test")
    {
        auto a = symd::Dimensions({3, 64, 128});
        REQUIRE(a.num_elements() == 3*64*128);
    }

    TEST_CASE("Dimensions: eltsise max test")
    {
        auto a = symd::Dimensions({3, 64, 128});
        auto b = symd::Dimensions({5, 32, 129});
        auto res = a.eltwise_max(b);

        REQUIRE(res.count() == 3);
        REQUIRE(res[0] == 5);
        REQUIRE(res[1] == 64);
        REQUIRE(res[2] == 129);
    }

    TEST_CASE("Dimensions: eltsise min test")
    {
        auto a = symd::Dimensions({3, 64, 128});
        auto b = symd::Dimensions({5, 32, 129});
        auto res = a.eltwise_min(b);

        REQUIRE(res.count() == 3);
        REQUIRE(res[0] == 3);
        REQUIRE(res[1] == 32);
        REQUIRE(res[2] == 128);
    }
}
