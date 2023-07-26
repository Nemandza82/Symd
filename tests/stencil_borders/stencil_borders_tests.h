#pragma once
#include "../test_helpers.h"


namespace tests
{
    static constexpr std::array<float, 9> kernel3x3 = {
        1, 0, -1,
        2, 0, -2,
        1, 0, -1
    };

    static constexpr std::array<float, 25> kernel5x5 = {
        2,  2,  4,  2,  2,
        1,  1,  2,  1,  1,
        0,  0,  0,  0,  0,
       -1, -1, -2, -1, -1,
       -2, -2, -4, -2, -2
    };

    static std::vector<float> input { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21 };

    struct SBTestData 
    {
        std::string name;

        int kernelSize;
        symd::Border border;
        float C;
        std::vector<float> expected_output;
    };

    TEST_CASE("Stencil - Border Types Test Cases")
    {
        auto [name, kernelSize, border, C, expected_output] = GENERATE(
            // 3x3 Kernel Test Cases
            SBTestData { "3x3 - mirror",           3, symd::Border::mirror, 0.0f, { 0, -8, -8, 0, 0, -8, -8, 0, 0, -8, -8, 0, 0, -8, -8, 0, 0, -8, -8, 0 } },
            SBTestData { "3x3 - constant=0",       3, symd::Border::constant, 0.0f, { -10, -6, -6, 13, -24, -8, -8, 28, -40, -8, -8, 44, -57, -8, -8, 61, -52, -6, -6, 55 } },
            SBTestData { "3x3 - constant=13",      3, symd::Border::constant, 13.0f, { 29, -6, -6, -26, 28, -8, -8, -24, 12, -8, -8, -8, -5, -8, -8, 9, -13, -6, -6, 16 } },
            SBTestData { "3x3 - replicate",        3, symd::Border::replicate, 0.0f, { -4, -8, -8, -4, -4, -8, -8, -4, -4, -8, -8, -4, -4, -8, -8, -4, -4, -8, -8, -4 } },
            SBTestData { "3x3 - mirror_replicate", 3, symd::Border::mirror_replicate, 0.0f, { -4, -8, -8, -4, -4, -8, -8, -4, -4, -8, -8, -4, -4, -8, -8, -4, -4, -8, -8, -4 } },
            // 5x5 Kernel Test Cases
            SBTestData { "5x5 - mirror",           5, symd::Border::mirror, 0.0f, { 0, 0, 0, 0, -144, -144, -144, -144, -252, -252, -252, -252, -150, -150, -150, -150, 0, 0, 0, 0 } },
            SBTestData { "5x5 - constant=0",       5, symd::Border::constant, 0.0f, { -101, -136, -139, -119, -142, -184, -186, -154, -168, -210, -210, -168, 10, 19, 21, 22, 133, 176, 179, 151 } },
            SBTestData { "5x5 - constant=5",       5, symd::Border::constant, 5.0f, { -41, -61, -64, -59, -102, -134, -136, -114, -168, -210, -210, -168, -30, -31, -29, -18, 73, 101, 104, 91 } },
            SBTestData { "5x5 - replicate",        5, symd::Border::replicate, 0.0f, { -120, -120, -120, -120, -192, -192, -192, -192, -252, -252, -252, -252, -210, -210, -210, -210, -138, -138, -138, -138 } },
            SBTestData { "5x5 - mirror_replicate", 5, symd::Border::mirror_replicate, 0.0f, { -72, -72, -72, -72, -192, -192, -192, -192, -252, -252, -252, -252, -210, -210, -210, -210, -78, -78, -78, -78 } }
        );

        // Logged on failure only
        INFO("Test Case: " + name);

        // Prepare 2D view to data to do 2D convolution
        symd::views::data_view<float, 2> input_2d(input.data(), 4, 5, 4);

        std::vector<float> output(input.size());
        symd::views::data_view<float, 2> output_2d(output.data(), 4, 5, 4);

        if (kernelSize == 3) 
        {
            // Do the 3x3 convolution. We also need 2D stencil view
            symd::map(output_2d, [&](const auto& x)
                {
                    return conv3x3_Kernel(x, kernel3x3.data());

                }, symd::views::stencil(input_2d, 3, 3, border, C));
        }
        else
        {
            // Do the 5x5 convolution. We also need 2D stencil view
            symd::map(output_2d, [&](const auto& x)
                {
                    return conv5x5_Kernel(x, kernel5x5.data());

                }, symd::views::stencil(input_2d, 5, 5, border, C));
        }

        // Verify
        requireNear(output, expected_output, 0.03f);
    }
}