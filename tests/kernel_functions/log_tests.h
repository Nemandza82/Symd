#pragma once
#include "../test_helpers.h"
#include <cmath>
#include <bitset>

namespace tests
{
    std::bitset<32> as_bits(float x)
    {
        int as_int = *((int*)(&x));
        return std::bitset<32>(as_int);
    }


    TEST_CASE("Mapping exp_part_of_float for 0..0f")
    {
        REQUIRE(symd::kernel::exp_part_of_float(0) == 0);
    }


    TEST_CASE("Mapping exp_part_of_float")
    {
        std::vector<float> input = { 0, 1e-45, 1e-44, 1e-43, 1e-42, 1e-41, 1e-40, 1e-39, 1e-38, 1e-37, 1e-36, 
            0.1, 0.2, 0.5,1, 2, 4, 8, 16, 32, 33, 63.1, 64, 127, 128, 129, 1e25, 1e36 };

        std::vector<float> map_output(input.size());

        symd::map_single_core(map_output, [](auto x)
            {
                return symd::kernel::exp_part_of_float(x);
            }, input);

        auto fp_out = helpers::apply_unary_op_to_vector<float>(input, [](auto x)
        {
            return symd::kernel::exp_part_of_float(x);
        });

        auto reference_out = helpers::apply_unary_op_to_vector<float>(input, [](auto x)
        {
            int int_exp;
            frexp(x, &int_exp);
            float std_result = pow(2, int_exp-1);

            if (x == 0)
                std_result = 0;

            return std_result;
        });

        helpers::require_near(map_output, reference_out, 1e-37f);
        helpers::require_near(fp_out, reference_out, 1e-37f);
    }


    TEST_CASE("Mapping fp_exp")
    {
        std::vector<float> input = { 0,
           -1e-38, -1e-37, -1e-36, -0.1, -0.2, -0.5, -1, -2, -4, -8, -16, -32, -33, -63.1, -64, -127, -128, -129, -1e25, -1e36,
            1e-38,  1e-37,  1e-36,  0.1,  0.2,  0.5,  1,  2,  4,  8,  16,  32,  33,  63.1,  64,  127,  128,  129,  1e25,  1e36 };

        std::vector<int> map_output(input.size());

        symd::map_single_core(map_output, [](auto x)
            {
                return symd::kernel::fp_exp(x);
            }, input);

        auto fp_out = helpers::apply_unary_op_to_vector<int>(input, [](auto x)
        {
            return symd::kernel::fp_exp(x);
        });

        auto reference_out = helpers::apply_unary_op_to_vector<int>(input, [](auto x)
        {
            int int_exp;
            frexp(x, &int_exp);
            int_exp -= 1;

            if (x == 0)
                return -127;

            return int_exp;
        });

        helpers::require_equal(map_output, reference_out);
        helpers::require_equal(fp_out, reference_out);
    }


    TEST_CASE("Mapping log")
    {
        std::vector<float> input = { 1e-45, 1e-44, 1e-43, 1e-42, 1e-41, 1e-40, 1e-39, 1e-38, 1e-37, 1e-36, 
            0.1, 0.2, 0.5, 1, 2, 4, 8, 16, 32, 33, 63.1, 64, 127, 128, 129, 1e25, 1e36 };

        std::vector<float> map_output(input.size());

        symd::map_single_core(map_output, [](auto x)
            {
                return symd::kernel::log(x);
            }, input);

        auto fp_out = helpers::apply_unary_op_to_vector<float>(input, [](auto x) { return symd::kernel::log(x); });
        auto reference_out = helpers::apply_unary_op_to_vector<float>(input, [](auto x){ return std::logf(x); });

        helpers::require_near(map_output, reference_out, 1e-4f);
        helpers::require_near(fp_out, reference_out, 1e-4f);
    }


    TEST_CASE("Mapping - log speed")
    {
        std::vector<float> input(2000000);
        std::vector<float> output(input.size());

        // initialize input
        //symd::map_single_core(input, [](){ return 1; });

        // Pass computation to measure time function. It fill execute it multiple times to measure time correctly.
        auto durationSymdSingleCore = helpers::measure_execution_time_ms([&]()
            {
                symd::map_single_core(output, [](const auto& x)
                    {
                        return symd::kernel::log(x);
                    }, input);
            }
        );

        // Measure time for simple for loop
        auto durationLoop = helpers::measure_execution_time_ms([&]()
            {
                for (size_t i = 0; i < input.size(); i++)
                    output[i] = std::logf(input[i]);
            }
        );

        std::cout << "log(x) - Loop             : " << durationLoop.count() << " ms" << std::endl;
        std::cout << "log(x) - symd_single_core : " << durationSymdSingleCore.count() << " ms" << std::endl;
    }
}
