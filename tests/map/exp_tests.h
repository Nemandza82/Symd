#pragma once
#include "../test_helpers.h"
#include <cmath>


namespace tests
{
    TEST_CASE("Mapping exp")
    {

        // Symd can work with std::array
        std::vector<float> input = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, -2, 3, -4, 5, -6, -7, 8, -9 };
        std::vector<float> output(input.size());
        std::vector<float> std_output(input.size());

        symd::map_single_core(output, [](auto x)
            {
                return symd::kernel::exp(x);
            }, input);

        for (size_t i=0; i<output.size(); i++)
        {
            std_output[i] = exp(input[i]);
        }

        for (size_t i=0; i<output.size(); i++)
        {
            std::cout << "input: " << input[i] << ", symd exp: " << output[i] << ", std exp: " << std_output[i] << std::endl;
        }

        std::cout << std::endl;
        //requireEqual(output.data(), { 2, 4, 6, 8, 10, 12, 14, 16, 18 });
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
