#pragma once
#include <future>
#include <algorithm>
#include <functional>
#include "internal/basic_views.h"
#include "internal/reduce_view.h"
#include "internal/multi_output.h"
#include "internal/sub_view.h"
#include "internal/stencil_view.h"


#ifdef SYMD_USE_TBB
    #include "tbb/parallel_for_each.h"
#elif defined(_WIN32) || defined(WIN32)
    #include <execution>
#endif

namespace symd
{
    namespace __internal__
    {
        template <typename Func, typename FirstInput, typename... Inputs>
        auto applyToFirstInput(Func&& func, const FirstInput& firstInput, const Inputs&... inputs)
        {
            return func(firstInput);
        }

        template <typename FirstInput, typename... Inputs>
        Region vectorRegion(const FirstInput& firstInput, const Inputs&... inputs)
        {
            std::vector<size_t> horBorders{ horisontalBorder(inputs)... };
            std::vector<size_t> verBorders{ verticalBorder(inputs)... };

            horBorders.push_back(horisontalBorder(firstInput));
            verBorders.push_back(verticalBorder(firstInput));

            size_t horBorder = *std::max_element(horBorders.begin(), horBorders.end());
            size_t verBorder = *std::max_element(verBorders.begin(), verBorders.end());

            auto width = getWidth(firstInput);
            auto heigth = getHeight(firstInput);

            Region region(verBorder, heigth-verBorder-1, horBorder, width-horBorder-1);

            auto lesserWidth = region.width() - (region.width() % SYMD_LEN);
            region.endCol = region.startCol + lesserWidth - 1;

            return region;
        }
    }

    /// <summary>
    /// Maps inputs to result using operation. Performs operation on single thread/core.
    /// </summary>
    /// <param name="result">Storing Result of the mapping operation.</param>
    /// <param name="operation">Operation to be performed on inputs.</param>
    /// <param name="...inputs">Input views for applying operation.</param>
    template <typename Output, typename Operation, typename... Inputs>
    void map_single_core(Output& result, Operation&& operation, Inputs&&... inputs)
    {
        auto width = __internal__::getWidth(result);
        auto height = __internal__::getHeight(result);

        auto vecRegion = __internal__::vectorRegion(inputs...);

        for (size_t i = 0; i < height; ++i)
        {
            size_t j = 0;

            // Inside the vector region we do vector operations
            if (i >= vecRegion.startRow && i <= vecRegion.endRow)
            {
                for (j = 0; j < vecRegion.startCol; ++j)
                {
                    auto pix = operation(__internal__::fetchData(inputs, i, j)...);
                    __internal__::saveData(result, pix, i, j);
                }

                for (; (j + __internal__::SYMD_LEN - 1) <= vecRegion.endCol; j += __internal__::SYMD_LEN)
                {
                    auto vecRes = operation(__internal__::fetchVecData(inputs, i, j)...);
                    __internal__::saveVecData(result, vecRes, i, j);
                }
            }

            // Outside of vector region we process remaining pixels...
            for (; j < width; ++j)
            {
                auto pix = operation(__internal__::fetchData(inputs, i, j)...);
                __internal__::saveData(result, pix, i, j);
            }
        }
    }

    /// <summary>
    /// Maps inputs to result using operation. Performs operation on mumltiple threads/cores.
    /// </summary>
    /// <param name="result">Storing Result of the mapping operation.</param>
    /// <param name="operation">Operation to be performed on inputs.</param>
    /// <param name="...inputs">Input views for applying operation.</param>
    template <typename Result, typename Operation, typename... Inputs>
    void map(Result& result, Operation&& operation, Inputs&&... inputs)
    {
        auto width = __internal__::getWidth(result);
        auto heigth = __internal__::getHeight(result);

        std::vector<__internal__::Region> regions;
        __internal__::Region(width, heigth).split(regions);

#ifdef SYMD_USE_TBB
        tbb::parallel_for_each(regions.begin(), regions.end(), [&](__internal__::Region& region)
            {
                auto subRes = __internal__::sub_view(result, region);
                map_single_core(subRes, operation, __internal__::sub_view(std::forward<Inputs>(inputs), region)...);
            });
#elif defined(_WIN32) || defined(WIN32)
        std::for_each(std::execution::par_unseq, regions.begin(), regions.end(), [&](__internal__::Region& region)
            {
                auto subRes = __internal__::sub_view(result, region);
                map_single_core(subRes, operation, __internal__::sub_view(std::forward<Inputs>(inputs), region)...);
            });
#else
        map_single_core(result, operation, inputs...);
#endif
    }
}
