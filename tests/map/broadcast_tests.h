#pragma once
#include "../test_helpers.h"

namespace tests
{
    TEST_CASE("Tensor 1")
    {
        symd::Tensor<float> input(symd::Dimensions({ 1, 1, 256, 64 }));
        symd::Tensor<float> output(input.shape());

        symd::map_single_core(input, [](auto x) { return x - x + 1.0f; }, input);

        symd::map_single_core(output, [](auto x) { return x * 2; }, input);

        std::cout << "--------------------------> Done" << std::endl;
        // requireEqual(output, { 2, 4, 6, 8, 10, 12, 14, 16, 18 });
    }
}
