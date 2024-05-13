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

    TEST_CASE("Mapping exp_part_of_float")
    {
        std::vector<float> input = { 0, 1e-45, 1e-44, 1e-43, 1e-42, 1e-41, 1e-40, 1e-39, 1e-38, 1e-37, 1e-36, 
            0.1, 0.2, 0.5,1, 2, 4, 8, 16, 32, 33, 63.1, 64, 127, 128, 129, 1e25, 1e36 };

        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::exp_part_of_float(x);
            }, input);

        REQUIRE(symd::kernel::exp_part_of_float(0) == 0);

        for (size_t i=0; i<input.size(); i++)
        {
            int int_exp;
            frexp(input[i], &int_exp);
            float std_result = pow(2, int_exp-1);

            if (input[i] == 0)
                std_result = 0;

            // Results need to be close
            REQUIRE(std::abs(output[i] - std_result) <= 1e-37);

            // Results need to be close
            REQUIRE(std::abs(symd::kernel::exp_part_of_float(input[i]) - std_result) <= 1e-37);
        }

        std::cout << std::endl;
    }

    TEST_CASE("Mapping fp_exp")
    {
        std::vector<float> input = { 0,
           -1e-38, -1e-37, -1e-36, -0.1, -0.2, -0.5, -1, -2, -4, -8, -16, -32, -33, -63.1, -64, -127, -128, -129, -1e25, -1e36,
            1e-38,  1e-37,  1e-36,  0.1,  0.2,  0.5,  1,  2,  4,  8,  16,  32,  33,  63.1,  64,  127,  128,  129,  1e25,  1e36 };

        std::vector<int> output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::fp_exp(x);
            }, input);

        for (size_t i=0; i<input.size(); i++)
        {
            int int_exp;
            frexp(input[i], &int_exp);
            int_exp -= 1;

            if (input[i] == 0)
            {
                REQUIRE(output[i] == -127);
                REQUIRE(symd::kernel::fp_exp(input[i]) == -127);
            }
            else
            {
                REQUIRE(output[i] == int_exp);
                REQUIRE(symd::kernel::fp_exp(input[i]) == int_exp);
            }
        }
    }


    TEST_CASE("Mapping log")
    {
        std::vector<float> input = { 0, 1e-45, 1e-44, 1e-43, 1e-42, 1e-41, 1e-40, 1e-39, 1e-38, 1e-37, 1e-36, 
            0.1, 0.2, 0.5, 1, 2, 4, 8, 16, 32, 33, 63.1, 64, 127, 128, 129, 1e25, 1e36 };

        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::log(x);
            }, input);


        for (size_t i=0; i<input.size(); i++)
        {
            int int_exp;
            std::frexp(input[i], &int_exp);
            auto symd_scalar_result = symd::kernel::log(input[i]);

            if (input[i] == 0.0f)
            {
                // Nans for 0 input
                REQUIRE(output[i] != output[i]);
                REQUIRE(symd::kernel::log(input[i]) != symd::kernel::log(input[i]));
            }
            else if (input[i] == 1.0f)
            {
                // 0 for 1 input
                REQUIRE(output[i] == 0.0f);
                REQUIRE(symd_scalar_result == 0.0f);
            }
            else
            {
                auto std_result = logf(input[i]);
                
                // Results need to be close
                REQUIRE(std::abs(output[i] - std_result) <= 1e-4);

                // Results need to be close
                REQUIRE(std::abs(symd_scalar_result - std_result) <= 1e-4);
            }
        }
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
                    output[i] = logf(input[i]);
            }
        );

        std::cout << "log(x) - Loop             : " << durationLoop.count() << " ms" << std::endl;
        std::cout << "log(x) - symd_single_core : " << durationSymdSingleCore.count() << " ms" << std::endl;
    }
}
