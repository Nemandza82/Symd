#pragma once
#include "../test_helpers.h"
#include "../../LibSymd/bfloat16.h"

namespace tests
{
    TEST_CASE("bfloat16 test 1")
    {
        float x = 3.14159f;
        float a = x;

        // Write zero over lower 16bits of float to see if value is preserved
        ((short*)(&a))[0] = 0;

        REQUIRE(std::abs(a - x) < 0.001f);

        float b = 0.0f;
        short lower_part = ((short*)(&b))[0];

        // Check if lower 16bits of 0.0f are still zeros
        REQUIRE(lower_part == 0);
    }

    TEST_CASE("bfloat16 test 2")
    {
        float x = 3.14159f;
        symd::bfloat16 a(x);

        REQUIRE(std::abs((float)a - x) < 0.001f);
    }

    TEST_CASE("Check if symd::bfloat16 takes 2 bytes")
    {
        std::vector<symd::bfloat16> bfloat16_vector = { 1.0f, 2.0f };

        uint64_t ptr0 = (uint64_t)((signed char*)(&bfloat16_vector[0]));
        uint64_t ptr1 = (uint64_t)((signed char*)(&bfloat16_vector[1]));

        REQUIRE(ptr1 - ptr0 == 2);
    }

    TEST_CASE("Mapping bfloat16")
    {
        std::vector<symd::bfloat16> input_1 = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        std::vector<symd::bfloat16> input_2 = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        std::vector<symd::bfloat16> output(input_2.size());

        symd::map_single_core(output, [](auto x, auto y) { return x + y; }, input_1, input_2);

        REQUIRE((float)output[0] == 2.0f);
        REQUIRE((float)output[1] == 4.0f);
        REQUIRE((float)output[2] == 6.0f);
        REQUIRE((float)output[3] == 8.0f);

        REQUIRE((float)output[4] == 10.0f);
        REQUIRE((float)output[5] == 12.0f);
        REQUIRE((float)output[6] == 14.0f);
        REQUIRE((float)output[7] == 16.0f);

        REQUIRE((float)output[8] == 18.0f);
    }

    TEST_CASE("Mapping bfloat16 abs")
    {
        std::vector<symd::bfloat16> input = { -5.0f, -1.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        std::vector<symd::bfloat16> output(input.size());

        symd::map_single_core(output, [](auto x) { return std::abs(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE((float)output[i] == std::abs((float)input[i]));

    }

    TEST_CASE("Mapping exec time bfloat16")
    {
        std::vector<float> input_1(1920*1080);
        std::vector<float> input_2(input_1.size());
        std::vector<float> output_float(input_1.size());

        // Pass computation to measure time function. It fill execute it multiple times to measure time correctly.
        auto durationFloat = helpers::executionTimeMs([&]()
            {
                symd::map_single_core(output_float, [](auto x, auto y)
                    {
                        return x + y;
                    }, input_1, input_2);
            }
        );

        std::cout << "Tensor addition float : " << durationFloat.count() << " ms" << std::endl;

        std::vector<symd::bfloat16> input_bf16_1(input_1.size());
        std::vector<symd::bfloat16> input_bf16_2(input_1.size());
        std::vector<symd::bfloat16> output_bf16(input_1.size());
        
        // Pass computation to measure time function. It fill execute it multiple times to measure time correctly.
        auto durationBf16 = helpers::executionTimeMs([&]()
            {
                symd::map_single_core(output_bf16, [](auto x, auto y)
                    {
                        return x + y;
                    }, input_bf16_1, input_bf16_2);
            }
        );

        std::cout << "Tensor addition bfloat16 : " << durationBf16.count() << " ms" << std::endl;
    }
}
