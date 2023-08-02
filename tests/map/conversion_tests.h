#pragma once
#include "../test_helpers.h"

namespace tests
{
    // NOTE! bfloat16 conversion tests are in bfloat16_tests.h

    TEST_CASE("Map: Convert float to double")
    {
        std::vector<float> input = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        std::vector<double> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<double>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (double)input[i]);
    }

    TEST_CASE("Map: Convert float to int")
    {
        std::vector<float> input = { 1.1f, 10e12, -10e12, 4.1f, 5.1f, 6.1f, 7.1f, 8.1f, 9.1f, 10e12, -10e12};
        std::vector<int> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<int>(x); }, input);

        REQUIRE(output[0] == 1);
        REQUIRE(output[1] == 2147483647);
        REQUIRE(output[2] == -2147483648);
        REQUIRE(output[3] == 4);

        REQUIRE(output[4] == 5);
        REQUIRE(output[5] == 6);
        REQUIRE(output[6] == 7);
        REQUIRE(output[7] == 8);

        REQUIRE(output[8] == 9);
        REQUIRE(output[9] == 2147483647);
        REQUIRE(output[10] == -2147483648);
    }
}
