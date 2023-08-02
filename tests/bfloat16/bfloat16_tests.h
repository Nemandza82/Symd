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

    TEST_CASE("Mapping bfloat 16")
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

    TEST_CASE("Map: Convert bfloat16 to float")
    {
        std::vector<symd::bfloat16> input = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<float>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (float)input[i]);
    }

    TEST_CASE("Map: Convert bfloat16 to double")
    {
        std::vector<symd::bfloat16> input = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        std::vector<double> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<double>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (double)input[i]);
    }

    TEST_CASE("Map: Convert bfloat16 to int")
    {
        std::vector<symd::bfloat16> input = { 1.1f, 10e12, -10e12, 4.1f, 5.1f, 6.1f, 7.1f, 8.1f, 9.1f, 10e12, -10e12 };
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

    TEST_CASE("Map: Convert bfloat16 to unsigned char")
    {
        std::vector<symd::bfloat16> input = { 1.1f, 300.1f, -100.1f, 4.1f, 5.1f, 6.1f, 7.1f, 8.1f, 9.1f, 300.f, -10.f };
        std::vector<unsigned char> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<unsigned char>(x); }, input);

        REQUIRE(output[0] == 1);
        REQUIRE(output[1] == 255);
        REQUIRE(output[2] == 0);
        REQUIRE(output[3] == 4);

        REQUIRE(output[4] == 5);
        REQUIRE(output[5] == 6);
        REQUIRE(output[6] == 7);
        REQUIRE(output[7] == 8);

        REQUIRE(output[8] == 9);
        REQUIRE(output[9] == 255);
        REQUIRE(output[10] == 0);
    }

    TEST_CASE("Map: Convert float to bfloat16")
    {
        std::vector<float> input = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
        std::vector<symd::bfloat16> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<symd::bfloat16>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE((float)output[i] == input[i]);
    }

    TEST_CASE("Map: Convert double to bfloat16")
    {
        std::vector<double> input = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
        std::vector<symd::bfloat16> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<symd::bfloat16>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE((double)output[i] == input[i]);
    }

    TEST_CASE("Map: Convert int to bfloat16")
    {
        std::vector<int> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::vector<symd::bfloat16> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<symd::bfloat16>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE((int)output[i] == input[i]);
    }

    TEST_CASE("Map: Convert unsigned char to bfloat16")
    {
        std::vector<unsigned char> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::vector<symd::bfloat16> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<symd::bfloat16>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE((unsigned char)output[i] == input[i]);
    }

    TEST_CASE("Mapping exec time bfloat16")
    {
        std::vector<float> input_1(1920*1080);
        std::vector<float> input_2(input_1.size());
        std::vector<float> output_float(input_1.size());

        // Pass computation to measure time function. It fill execute it multiple times to measure time correctly.
        auto durationFloat = executionTimeMs([&]()
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
        auto durationBf16 = executionTimeMs([&]()
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
