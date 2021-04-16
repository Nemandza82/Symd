#pragma once
#include <execution>
#include <future>
#include <algorithm>
#include <functional>
#include "internal/basic_views.h"
#include "internal/multi_output.h"
#include "internal/stencil_view.h"


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
        Region safeRegion(const FirstInput& firstInput, const Inputs&... inputs)
        {
            return Region(getWidth(firstInput), getHeight(firstInput));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="Output"></typeparam>
    /// <typeparam name="Operation"></typeparam>
    /// <typeparam name="...Inputs"></typeparam>
    /// <param name="res"></param>
    /// <param name="operation"></param>
    /// <param name="...inputs"></param>
    template <typename Output, typename Operation, typename... Inputs>
    void map_single_core(Output& result, Operation&& operation, Inputs&&... inputs)
    {
        //auto width = __internal__::applyToFirstInput([](auto x) { return __internal__::getWidth(x); }, inputs...);
        //auto heigth = __internal__::applyToFirstInput([](auto x) { return __internal__::getHeight(x); }, inputs...);
        auto width =  __internal__::getWidth(result);
        auto heigth = __internal__::getHeight(result);

        __internal__::Region safeRegion = __internal__::Region(width, heigth);

        for (size_t i = 0; i < heigth; ++i)
        {
            size_t j = 0;

            // Here we are in safe region so we are doing vector operations
            if (i >= safeRegion.startRow && i <= safeRegion.endRow)
            {
                for (j = 0; j < safeRegion.startCol; ++j)
                {
                    auto pix = operation(__internal__::fetchData(inputs, i, j)...);
                    __internal__::saveData(result, pix, i, j);
                }

                for (; (j + __internal__::SYMD_LEN - 1) <= safeRegion.endCol; j += __internal__::SYMD_LEN)
                {
                    auto vecRes = operation(__internal__::fetchVecData(inputs, i, j)...);
                    __internal__::saveVecData(result, vecRes, i, j);
                }
            }

            for (; j <= safeRegion.endCol; ++j)
            {
                auto pix = operation(__internal__::fetchData(inputs, i, j)...);
                __internal__::saveData(result, pix, i, j);
            }
        }
    }

    template <typename Result, typename Operation, typename... Inputs>
    void map(Result& result, Operation&& operation, Inputs&&... inputs)
    {
        auto width = __internal__::getWidth(result);
        auto heigth = __internal__::getHeight(result);

        std::vector<__internal__::Region> regions;
        __internal__::Region(width, heigth).split(regions);

        std::for_each(std::execution::par_unseq, regions.begin(), regions.end(), [&](__internal__::Region& region)
            {
                auto subRes = views::sub_view(result, region);
                map_single_core(subRes, operation, views::sub_view(std::forward<Inputs>(inputs), region)...);
            });
    }
}
