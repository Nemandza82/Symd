#include "catch.h"
#include <iostream>
#include "include/symd.h"
#include <chrono>
#include <algorithm>
#include <random>

namespace tests
{
    // TODO: Repeated from test_symd.cpp, do something about it.
    template <typename StencilView, typename DataType>
    auto conv3x3_Kernel(const StencilView& sv, const DataType* kernel)
    {
        return
            sv(-1, -1) * kernel[0] + sv(-1, 0) * kernel[1] + sv(-1, 1) * kernel[2] +
            sv(0, -1) * kernel[3] + sv(0, 0) * kernel[4] + sv(0, 1) * kernel[5] +
            sv(1, -1) * kernel[6] + sv(1, 0) * kernel[7] + sv(1, 1) * kernel[8];
    }

    // TODO: Repeated from test_symd.cpp, do something about it.
    template <typename T>
    static void requireNear(const std::vector<T>& data, const std::vector<T>& ref, T eps)
    {
        REQUIRE(data.size() == ref.size());

        for (size_t i = 0; i < ref.size(); i++)
            REQUIRE(std::abs(data[i] - ref[i]) < eps);
    }

    static constexpr float epsilon = 0.03f;

    static constexpr std::array<float, 9> kernel3x3 = {
        1, 0, -1,
        2, 0, -2,
        1, 0, -1
    };

    //static constexpr std::array<float, 25> kernel5x5 = {
    //    2,  2,  4,  2,  2,
    //    1,  1,  2,  1,  1,
    //    0,  0,  0,  0,  0,
    //   -1, -1, -2, -1, -1,
    //   -2, -2, -4, -2, -2
    //};

    static std::vector<float> input { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21 };

    struct SBTestData 
    {
        symd::Border border;
        float C;
        std::vector<float> expected_output;
    };

    TEST_CASE("Stencil - Border Test Cases")
    {
        auto [border, C, expected_output] = GENERATE(
            SBTestData { symd::Border::mirror, 0.0f, { 0, -8, -8, 0, 0, -8, -8, 0, 0, -8, -8, 0, 0, -8, -8, 0, 0, -8, -8, 0 } },
            SBTestData { symd::Border::constant, 0.0f, { -10, -6, -6, 13, -24, -8, -8, 28, -40, -8, -8, 44, -57, -8, -8, 61, -52, -6, -6, 55 } },
            SBTestData { symd::Border::constant, 13.0f, { 29, -6, -6, -26, 28, -8, -8, -24, 12, -8, -8, -8, -5, -8, -8, 9, -13, -6, -6, 16 } }
        );

        // Prepare 2D view to data to do 2D convolution
        symd::views::data_view<float, 2> input_2d(input.data(), 4, 5, 4);

        std::vector<float> output(input.size());
        symd::views::data_view<float, 2> output_2d(output.data(), 4, 5, 4);

        // Do the convolution. We also need 2D stencil view
        symd::map(output_2d, [&](const auto& x)
            {
                return conv3x3_Kernel(x, kernel3x3.data());

            }, symd::views::stencil(input_2d, 3, 3, border, C));

        // Verify
        requireNear(output, expected_output, 0.03f);
    }
}