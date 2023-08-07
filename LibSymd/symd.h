#pragma once
#include <future>
#include <algorithm>
#include <functional>
#include  <iostream>
#include "dimensions.h"
#include "internal/basic_views.h"
#include "kernel/all_ops.h"
#include "internal/sub_view.h"
#include "internal/stencil_view.h"
#include "internal/multi_output.h"

#ifdef SYMD_USE_TBB
    #include "tbb/parallel_for_each.h"
#elif defined(_WIN32) || defined(WIN32)
    #include <execution>
#endif


namespace symd::__internal__
{ 
    template <typename Func, typename FirstInput, typename... Inputs>
    auto applyToFirstInput(Func&& func, const FirstInput& firstInput, const Inputs&... inputs)
    {
        return func(firstInput);
    }

    template <typename FirstInput, typename... Inputs>
    Region vectorRegion(const FirstInput& firstInput, const Inputs&... inputs)
    {
        std::vector<Dimensions> borders{ getBorder(inputs)... };
        auto maxBorders = getBorder(firstInput);

        for (const auto& border : borders)
            maxBorders = maxBorders.eltwise_max(border);

        auto shape = getShape(firstInput);
        auto startShape = shape.zeros_like() + maxBorders;
        auto endShape = shape - maxBorders - 1;

        Region region(startShape, endShape);
        return region.align_with_symd_len(SYMD_LEN);
    }

    template <typename Output, typename Operation, typename... Inputs>
    void map_single_core_impl(
        Output& result, 
        Operation&& operation, 
        Dimensions shape, 
        Region vecRegion,
        Dimensions proc_coord,
        int proc_dim,
        bool inside_vec_region,
        Inputs&&... inputs)
    {
        // Last dim
        if (proc_dim == shape.num_dims() - 1)
        {
            int64_t i = 0;

            for (; i < vecRegion.startCoord[proc_dim]; ++i)
            {
                proc_coord.set_ith_dim(proc_dim, i);
                auto pix = operation(__internal__::fetchData(inputs, proc_coord)...);
                __internal__::saveData(result, pix, proc_coord);
            }

            if (inside_vec_region)
            {
                for (; (i + __internal__::SYMD_LEN - 1) <= vecRegion.endCoord[proc_dim]; i += __internal__::SYMD_LEN)
                {
                    proc_coord.set_ith_dim(proc_dim, i);
                    auto vecRes = operation(__internal__::fetchVecData(inputs, proc_coord)...);
                    __internal__::saveVecData(result, vecRes, proc_coord);
                }
            }

            for (; i < shape[proc_dim]; ++i)
            {
                proc_coord.set_ith_dim(proc_dim, i);
                auto pix = operation(__internal__::fetchData(inputs, proc_coord)...);
                __internal__::saveData(result, pix, proc_coord);
            }
        }
        else
        {
            for (size_t i = 0; i < shape[proc_dim]; ++i)
            {
                proc_coord.set_ith_dim(proc_dim, i);

                bool is_inside_vec_region = inside_vec_region &&
                    (i >= vecRegion.startCoord[proc_dim]) &&
                    (i <= vecRegion.endCoord[proc_dim]);

                map_single_core_impl(
                    result,
                    std::forward<Operation>(operation),
                    shape,
                    vecRegion,
                    proc_coord,
                    proc_dim + 1,
                    is_inside_vec_region,
                    std::forward<Inputs>(inputs)... );
            }
        }
    }
} // symd::__internal__

namespace symd
{
    /// <summary>
    /// Maps inputs to result using operation. Performs operation on single thread/core.
    /// </summary>
    /// <param name="result">Storing Result of the mapping operation.</param>
    /// <param name="operation">Operation to be performed on inputs.</param>
    /// <param name="...inputs">Input views for applying operation.</param>
    template <typename Output, typename Operation, typename... Inputs>
    void map_single_core(Output& result, Operation&& operation, Inputs&&... inputs)
    {
        auto shape = __internal__::getShape(result);
        auto vecRegion = __internal__::vectorRegion(inputs...);

        __internal__::map_single_core_impl(
            result,
            std::forward<Operation>(operation),
            shape,
            vecRegion,
            shape.zeros_like(),
            0,
            true,
            std::forward<Inputs>(inputs)... );
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
        auto shape = __internal__::getShape(result);

        std::vector<__internal__::Region> regions;
        __internal__::Region(shape).split(regions);

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
} // namespace symd
