#pragma once
#include "../test_helpers.h"

namespace tests
{
    ///////////////////////////////////////////////////////////////////////
    // float to T conversions
    ///////////////////////////////////////////////////////////////////////

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

    TEST_CASE("Map: Convert float to unsigned char")
    {
        std::vector<float> input = { 1.1f, 500.1f, -100.2f, 4.1f, 5.1f, 6.1f, 7.1f, 8.1f, 9.1f, 500.1f, -500.f};
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


    ///////////////////////////////////////////////////////////////////////
    // double to T conversions
    ///////////////////////////////////////////////////////////////////////

    TEST_CASE("Map: Convert double to float")
    {
        std::vector<double> input = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<float>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (float)input[i]);
    }

    TEST_CASE("Map: Convert double to int")
    {
        std::vector<double> input = { 1.1f, 10e12, -10e12, 4.1f, 5.1f, 6.1f, 7.1f, 8.1f, 9.1f, 10e12, -10e12};
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

    TEST_CASE("Map: Convert double to unsigned char")
    {
        std::vector<double> input = { 1.1, 500.1, -100.2, 4.1, 5.1, 6.1, 7.1, 8.1, 9.1, 500.1, -500.0};
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

    TEST_CASE("Map: Convert double to bfloat16")
    {
        std::vector<double> input = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
        std::vector<symd::bfloat16> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<symd::bfloat16>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE((double)output[i] == input[i]);
    }


    ///////////////////////////////////////////////////////////////////////
    // int to T conversions
    ///////////////////////////////////////////////////////////////////////

    TEST_CASE("Map: Convert int to float")
    {
        std::vector<int> input = { 1, 500, -100, 4, 5, 6, 7, 8, 9, 500, -500};
        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<float>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (float)input[i]);
    }

    TEST_CASE("Map: Convert int to double")
    {
        std::vector<int> input = { 1, 500, -100, 4, 5, 6, 7, 8, 9, 500, -500};
        std::vector<double> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<double>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (double)input[i]);
    }

    TEST_CASE("Map: Convert int to unsigned char")
    {
        std::vector<int> input = { 1, 500, -100, 4, 5, 6, 7, 8, 9, 500, -500};
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

    TEST_CASE("Map: Convert int to bfloat16")
    {
        std::vector<int> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::vector<symd::bfloat16> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<symd::bfloat16>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE((int)output[i] == input[i]);
    }


    ///////////////////////////////////////////////////////////////////////
    // unsigned char to T conversions
    ///////////////////////////////////////////////////////////////////////

    TEST_CASE("Map: Convert unsigned char to float")
    {
        std::vector<unsigned char> input = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255 };
        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<float>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (float)input[i]);
    }

    TEST_CASE("Map: Convert unsigned char to double")
    {
        std::vector<unsigned char> input = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255 };
        std::vector<double> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<double>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (double)input[i]);
    }

    TEST_CASE("Map: Convert unsigned char to int")
    {
        std::vector<unsigned char> input = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255 };
        std::vector<int> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<int>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE(output[i] == (int)input[i]);
    }

    TEST_CASE("Map: Convert unsigned char to bfloat16")
    {
        std::vector<unsigned char> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::vector<symd::bfloat16> output(input.size());

        symd::map_single_core(output, [](auto x) { return symd::kernel::convert_to<symd::bfloat16>(x); }, input);

        for (size_t i = 0; i < input.size(); i++)
            REQUIRE((unsigned char)output[i] == input[i]);
    }


    ///////////////////////////////////////////////////////////////////////
    // bfloat16 to T conversions
    ///////////////////////////////////////////////////////////////////////

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
}
