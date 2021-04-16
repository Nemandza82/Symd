#pragma once
#include <execution>
#include <future>
#include <algorithm>
#include "internal/symd_register.h"
#include <functional>
#include "internal/region.h"
#include "views.h"



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

        template <typename Input>
        auto fetchData(const Input& input, size_t row, size_t col)
        {
            auto ptr = getDataPtr(input, row, col);
            return *ptr;
        }

        template <typename Input>
        auto fetchVecData(const Input& input, size_t row, size_t col)
        {
            auto* ptr = getDataPtr(input, row, col);

            return SymdRegister<std::decay_t<decltype(*ptr)>>(ptr);
        }

        template <typename Output>
        auto saveData(Output& out, size_t row, size_t col)
        {
            return *getDataPtr(out, row, col);
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
            auto* dstPtr = __internal__::getDataPtr(result, i, j);

            // Here we are in safe region so we are doing vector operations
            if (i >= safeRegion.startRow && i <= safeRegion.endRow)
            {
                for (j = 0; j < safeRegion.startCol; ++j, ++dstPtr)
                {
                    auto pix = operation(__internal__::fetchData(inputs, i, j)...);
                    *dstPtr = pix;
                }

                for (; (j + __internal__::SYMD_LEN - 1) <= safeRegion.endCol; j += __internal__::SYMD_LEN, dstPtr += __internal__::SYMD_LEN)
                {
                    auto vecRes = operation(__internal__::fetchVecData(inputs, i, j)...);
                    vecRes.store(dstPtr);
                }
            }

            for (; j <= safeRegion.endCol; ++j, ++dstPtr)
            {
                auto pix = operation(__internal__::fetchData(inputs, i, j)...);
                *dstPtr = pix;
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

        std::for_each(std::execution::par, regions.begin(), regions.end(), [&](__internal__::Region& region)
            {
                auto subRes = subView(result, region);
                map_single_core(subRes, operation, subView(std::forward<Inputs>(inputs), region)...);
            });
    }
}
