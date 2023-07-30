#pragma once
#include "../test_helpers.h"


namespace tests
{
    TEST_CASE("Reduction simple test")
    {
        std::vector<float> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
        auto input_shape = symd::Dimensions({ (int64_t)input.size() });

        // Create reduce view which is enabling us to perform reduction.
        // Reduce view size must be equal to input size
        // Reduction operation is summing in this case
        auto sum = symd::views::reduce_view(input_shape, 0.0f, [](auto x, auto y)
            {
                return x + y;
            });

        // We are performing reduction by mapping to reduce view.
        // This enables us to apply some map (transorm0 operation prior to reducing.
        symd::map(sum, [](auto x) { return x * 2; }, input);

        // Get funal result from reduce view
        auto res = sum.getResult();
        REQUIRE(res ==  342);
    }

    /* TEST_CASE("Reduction - many elements")
    {
        int64_t width = 1920;
        int64_t height = 1080;

        std::vector<int> input(width * height);
        randomizeData(input);

        symd::views::data_view<int, 2> input_2d(input.data(), width, height, width);
        int res;

        auto durationMap = executionTimeMs([&]()
            {
                auto sum = symd::views::reduce_view(width, height, (int)0, [](auto x, auto y)
                    {
                        return x + y;
                    });

                symd::map(sum, [](auto x) { return x; }, input_2d);

                res = sum.getResult();
            });

        int resLoop = 0;

        auto durationLoop = executionTimeMs([&]()
            {
                // Simple loop
                resLoop = 0;

                for (auto x : input)
                    resLoop += x;
            });

        // Get funal result from reduce view
        REQUIRE(res == resLoop);

        std::cout << "Simple reduction - Loop: " << durationLoop.count() << " ms" << std::endl;
        std::cout << "Simple reduction - Symd: " << durationMap.count() << " ms" << std::endl << std::endl;
    }

    TEST_CASE("Reduction - multiple outputs")
    {
        size_t width = 1920;
        size_t height = 1080;

        std::vector<int> input(width * height);
        randomizeData(input);

        symd::views::data_view<int, 2> input_2d(input.data(), width, height, width);

        auto sumX = symd::views::reduce_view(width, height, (int)0, [](auto x, auto y)
            {
                return x + y;
            });

        auto sumY = symd::views::reduce_view(width, height, (int)0, [](auto x, auto y)
            {
                return x + y;
            });

        auto outTuple = std::tie(sumX, sumY);

        symd::map(outTuple, [](auto x)
            {
                return std::array{ x, 2 * x };

            }, input_2d);

        auto resX = sumX.getResult();
        auto resY = sumY.getResult();

        REQUIRE(resY == 2 * resX);
    }


    TEST_CASE("Reduction - multiple outputs 2")
    {
        size_t width = 1920;
        size_t height = 1080;

        std::vector<int> input(width * height);
        randomizeData(input);

        symd::views::data_view<int, 2> input_2d(input.data(), width, height, width);

        auto reduceOpp = [](auto x, auto y) { return x + y; };

        auto out = std::array{ 
            symd::views::reduce_view(width, height, (int)0, reduceOpp),
            symd::views::reduce_view(width, height, (int)0, reduceOpp)
        };

        symd::map(out, [](auto x)
            {
                return std::array{ x, 2 * x };

            }, input_2d);

        auto resX = out[0].getResult();
        auto resY = out[1].getResult();

        REQUIRE(resY == 2 * resX);
    }*/
}

