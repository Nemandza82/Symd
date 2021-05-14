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

    TEST_CASE("Stencil - Border Type Mirror")
    {
        std::vector<float> input { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21 };

        // Kernel for convolution
        std::array<float, 9> kernel = {
             0,    -1.f,    0,
            -1.f,   4.f,   -1.f,
             0,    -1.f,    0
        };

        // Prepare 2D view to data to do 2D convolution
        symd::views::data_view<float, 2> input_2d(input.data(), 4, 5, 4);

        std::vector<float> output(input.size());
        symd::views::data_view<float, 2> output_2d(output.data(), 4, 5, 4);

        symd::map(output_2d, [&](const auto& x)
            {
                return conv3x3_Kernel(x, kernel.data());

            }, symd::views::stencil(input_2d, 3, 3));

        std::vector<float> expected_output { -10.0f, -8.0f, -8.0f, -6.0f,
                                             -2.0f, 0.0f, 0.0f, 2.0f,
                                             -2.0f, 0.0f, 0.0f, 2.0f,
                                             -3.0f, -1.0f, -1.0f, 1.0f,
                                              8.0f, 10.0f, 10.0f, 12.0f };

        requireNear(output, expected_output, 0.03f);
    }

    // GENERATOS example!
    //struct TestData
    //{
    //    Position startPos;
    //    Position expectedMove;
    //};

    //TEST_CASE("Test legal moves on empty 2x1 board")
    //{
    //    Board board{ 2, 1 };
    //    auto testData = GENERATE(TestData{ {0, 0}, {1, 0} }, TestData{ {1, 0}, {0, 0} });
    //    auto lagalMoves = board.getLegalMoves(testData.startPos);
    //    REQUIRE(lagalMoves[0] == testData.expectedMove);
    //}
}