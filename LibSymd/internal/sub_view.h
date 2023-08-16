#pragma once
#include "basic_views.h"
#include "region.h"
#include <algorithm>


namespace symd::__internal__
{
    template <typename View>
    struct SubView
    {
        View _underlyingView;
        Region _region;

        SubView(View&& view, const __internal__::Region& region)
            : _underlyingView(std::forward<View>(view))
            , _region(region)
        {
        }
    };

    template <typename View>
    Dimensions getShape(const SubView<View>& subView)
    {
        return subView._region.getShape();
    }

    template <typename View>
    Dimensions getPitch(const SubView<View>& subView)
    {
        return getPitch(subView._underlyingView);
    }

    template <typename View>
    auto fetchData(const SubView<View>& subView, const Dimensions& coords)
    {
        return fetchData(
            subView._underlyingView,
            subView._region.startCoord + coords);
    }

    template <typename View>
    auto fetchVecData(const SubView<View>& subView, const Dimensions& coords)
    {
        return fetchVecData(
            subView._underlyingView,
            subView._region.startCoord + coords);
    }

    template <typename View, typename DataType>
    void saveData(SubView<View>& subView, const DataType& element, const Dimensions& coords)
    {
        saveData(
            subView._underlyingView,
            element,
            subView._region.startCoord + coords);
    }

    template <typename View, typename DataType>
    void saveVecData(SubView<View>& subView, const SymdRegister<DataType>& element, const Dimensions& coords)
    {
        saveVecData(
            subView._underlyingView,
            element,
            subView._region.startCoord + coords);
    }

    template <typename View>
    Dimensions getBorder(const SubView<View>& subView)
    {
        auto underlyingShape = getShape(subView._underlyingView);
        auto underlyingBorder = getBorder(subView._underlyingView);

        return underlyingBorder;

        // auto result = underlyingBorder.zeros_like();

        // for (int i = 0; i < underlyingBorder.num_dims(); i++)
        // {
        //     if (underlyingBorder[i] > subView._region.startCoord[i])
        //         result.set_ith_dim(i, std::max(result[i], underlyingBorder[i] - subView._region.startCoord[i]));

        //     size_t rightDistance = underlyingShape[i] - subView._region.endCoord[i] - 1;

        //     if (underlyingBorder[i] > rightDistance)
        //         result.set_ith_dim(i, std::max(result[i], underlyingBorder[i] - rightDistance);
        // }

        // return result;
    }

    template <typename View>
    auto sub_view(View&& view, const Region& region)
    {
        return __internal__::SubView<View>(std::forward<View>(view), region);
    }
}


namespace symd::views
{
    /// <summary>
    /// Returns sub-view of underlying view. 
    /// </summary>
    /// <param name="view">Underlying view. </param>
    /// <param name="startShape">Start shape for sub-view.</param>
    /// <param name="endShape">End shape for sub-view.</param>
    template <typename View>
    auto sub_view(View&& view, const Dimensions& startShape, const Dimensions& endShape)
    {
        return __internal__::sub_view(std::forward<View>(view), __internal__::Region(startShape, endShape));
    }
}
