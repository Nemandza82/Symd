#pragma once
#include "../test_helpers.h"
#include <cmath>


namespace tests
{
    TEST_CASE("Mapping fastpow2 float")
    {
        // Symd can work with std::array
        std::vector<int> input = { 80, 70, 60, 50, 40, 30, 20, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 
            -1, -2, 3, -4, 5, -6, -7, 8, -9, -20, -30, -40, -50, -60, -70, -80 };

        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::__internal_exp::fastpow2<float>(x);
            }, input);

        for (size_t i=0; i<output.size(); i++)
        {
            float std_output = pow(2, input[i]);
            float out_scalar = symd::kernel::__internal_exp::fastpow2<float>(input[i]);

            REQUIRE(std::abs(output[i] - std_output) <= 1e-7);
            REQUIRE(std::abs(out_scalar - std_output) <= 1e-7);
        }
    }

    TEST_CASE("Mapping fastpow2 double")
    {
        // Symd can work with std::array
        std::vector<int> input = { 80, 70, 60, 50, 40, 30, 20, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 
            -1, -2, 3, -4, 5, -6, -7, 8, -9, -20, -30, -40, -50, -60, -70, -80 };

        std::vector<double> output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::__internal_exp::fastpow2<double>(x);
            }, input);

        for (size_t i=0; i<output.size(); i++)
        {
            double std_output = pow(2, input[i]);
            double out_scalar = symd::kernel::__internal_exp::fastpow2<double>(input[i]);

            double rel_err = std::abs(output[i] - std_output) / std_output;
            double rel_err_scallar = std::abs(out_scalar - std_output) / std_output;

            REQUIRE(rel_err <= 1e-7);
            REQUIRE(rel_err_scallar <= 1e-7);
        }
    }

    TEST_CASE("Mapping fastpow2f float")
    {
        // Symd can work with std::array
        std::vector<float> input = { 80.1, 70.2, 60.3, 50.4, 40.5, 30.6, 20.7, 10.8, 9.9, 8.1, 7.2, 6.3, 5.4, 4.0, 3.5, 2.6, 1.7, 1.0,
            0.8, 0.5, 0.1, 0, -0.1, -0.5, -0.8, 1.0, -1.1, -2.2, 3.3, -4.4, 5.5, -6.6, -7.7, 8.8, -9.9, 
            -20.1, -30.2, -40.3, -50.4, -60.5, -70.6, -80.7 };

        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::__internal_exp::fastpow2f(x);
            }, input);

        for (size_t i=0; i<output.size(); i++)
        {
            float std_output = pow(2, input[i]);
            float out_scalar = symd::kernel::__internal_exp::fastpow2f(input[i]);
            
            float rel_err = std::abs(output[i] - std_output) / std_output;
            float rel_err_scallar = std::abs(out_scalar - std_output) / std_output;

            REQUIRE(rel_err <= 1e-4);
            REQUIRE(rel_err_scallar <= 1e-4);
        }
    }

    TEST_CASE("Mapping fastpow2f double")
    {
        // Symd can work with std::array
        std::vector<double> input = { 80.1, 70.2, 60.3, 50.4, 40.5, 30.6, 20.7, 10.8, 9.9, 8.1, 7.2, 6.3, 5.4, 4.0, 3.5, 2.6, 1.7, 1.0,
            0.8, 0.5, 0.1, 0, -0.1, -0.5, -0.8, 1.0, -1.1, -2.2, 3.3, -4.4, 5.5, -6.6, -7.7, 8.8, -9.9, 
            -20.1, -30.2, -40.3, -50.4, -60.5, -70.6, -80.7 };

        std::vector<double> output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::__internal_exp::fastpow2f(x);
            }, input);

        for (size_t i=0; i<output.size(); i++)
        {
            double std_output = pow(2, input[i]);
            double out_scalar = symd::kernel::__internal_exp::fastpow2f(input[i]);
            
            double rel_err = std::abs(output[i] - std_output) / std_output;
            double rel_err_scallar = std::abs(out_scalar - std_output) / std_output;

            // std::cout 
            //     << "input: " << input[i] 
            //     << ", symd fastpow2f: " << output[i] 
            //     << ", std pow2: " << std_output
            //     << ", rel err: " << rel_err
            //     << ", rel err scalar: " << rel_err_scallar
            //     << std::endl;

            REQUIRE(rel_err <= 1e-4);
            REQUIRE(rel_err_scallar <= 1e-4);
        }
    }

    TEST_CASE("Mapping exp")
    {
        // Symd can work with std::array
        std::vector<float> input = { 80, 70, 60, 50, 40, 30, 20, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0.5, 0.1, 0, 
            -0.1, -0.5, -1, -2, 3, -4, 5, -6, -7, 8, -9, -20, -30, -40, -50, -60, -70, -80 };

        std::vector<float> output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::exp(x);
            }, input);

        for (size_t i=0; i<output.size(); i++)
        {
            float std_output = exp(input[i]);
            float out_scalar = symd::kernel::exp(input[i]);

            float rel_err = std::abs(output[i]  - std_output) / std_output;
            float rel_err_scallar = std::abs(out_scalar - std_output) / std_output;

            // std::cout 
            //     << "input: " << input[i] 
            //     << ", symd exp: " << output[i] 
            //     << ", std pow2: " << std_output
            //     << ", rel err: " << rel_err
            //     << std::endl;

            REQUIRE(rel_err <= 3e-4);
            REQUIRE(rel_err_scallar <= 3e-4);
        }
    }

    TEST_CASE("Mapping - exp speed")
    {
        std::vector<float> input(2000000);
        std::vector<float> output(input.size());

        // initialize input
        //symd::map_single_core(input, [](){ return 1; });

        // Pass computation to measure time function. It fill execute it multiple times to measure time correctly.
        auto durationSymdSingleCore = executionTimeMs([&]()
            {
                symd::map_single_core(output, [](const auto& x)
                    {
                        return symd::kernel::exp(x);
                    }, input);
            }
        );

        // Measure time for simple for loop
        auto durationLoop = executionTimeMs([&]()
            {
                for (size_t i = 0; i < input.size(); i++)
                    output[i] = exp(input[i]);
            }
        );

        std::cout << "exp(x) - Loop             : " << durationLoop.count() << " ms" << std::endl;
        std::cout << "exp(x) - symd_single_core : " << durationSymdSingleCore.count() << " ms" << std::endl;
    }
}
