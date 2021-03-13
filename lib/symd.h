#pragma once
#include "symd_register.h"
#include <functional>
#include "region.h"
#include "views.h"


namespace symd
{
    namespace internal
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
        auto accessData(Input input, size_t row, size_t col)
        {
            return *getDataPtr(input, row, col);
        }

        template <typename Input>
        auto accessDataVec(Input input, size_t row, size_t col)
        {
            auto* ptr = getDataPtr(input, row, col);

            return SymdRegister<std::decay_t<decltype(*ptr)>>(ptr);
        }

        template <typename Input>
        auto saveData(Input input, size_t row, size_t col)
        {
            return *getDataPtr(input, row, col);
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
        auto width = internal::applyToFirstInput([](auto x) { return internal::getWidth(x); }, inputs...);
        auto heigth = internal::applyToFirstInput([](auto x) { return internal::getHeight(x); }, inputs...);

        internal::Region safeRegion = internal::safeRegion(inputs...);

        for (size_t i = 0; i < heigth; ++i)
        {
            size_t j = 0;
            auto* dstPtr = internal::getDataPtr(result, i, j);

            // Here we are in safe region so we are doing vector operations
            if (i >= safeRegion.startRow && i <= safeRegion.endRow)
            {
                for (j = 0; j < safeRegion.startCol; ++j, ++dstPtr)
                {
                    auto pix = operation(internal::accessData(inputs, i, j)...);
                    *dstPtr = pix;
                }

                for (; j + internal::SYMD_LEN - 1 <= safeRegion.endCol; j += internal::SYMD_LEN, dstPtr += internal::SYMD_LEN)
                {
                    auto vecRes = operation(internal::accessDataVec(inputs, i, j)...);
                    vecRes.store(dstPtr);
                }
            }

            for (; j < safeRegion.endCol; ++j, ++dstPtr)
            {
                auto pix = operation(internal::accessData(inputs, i, j)...);
                *dstPtr = pix;
            }
        }
    }
}
