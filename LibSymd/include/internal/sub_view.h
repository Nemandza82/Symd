#pragma once
#include "basic_views.h"
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
    size_t getWidth(const SubView<View>& subView)
    {
        return subView._region.width();
    }

    template <typename View>
    size_t getHeight(const SubView<View>& subView)
    {
        return subView._region.height();
    }

    template <typename View>
    size_t getPitch(const SubView<View>& subView)
    {
        return getPitch(subView._underlyingView);
    }

    template <typename View>
    auto fetchData(const SubView<View>& subView, size_t row, size_t col)
    {
        return fetchData(
            subView._underlyingView,
            subView._region.startRow + row,
            subView._region.startCol + col);
    }

    template <typename View>
    auto fetchVecData(const SubView<View>& subView, size_t row, size_t col)
    {
        return fetchVecData(
            subView._underlyingView,
            subView._region.startRow + row,
            subView._region.startCol + col);
    }

    template <typename View, typename DataType>
    void saveData(SubView<View>& subView, const DataType& element, size_t row, size_t col)
    {
        saveData(
            subView._underlyingView,
            element,
            subView._region.startRow + row,
            subView._region.startCol + col);
    }

    template <typename View, typename DataType>
    void saveVecData(SubView<View>& subView, const SymdRegister<DataType>& element, size_t row, size_t col)
    {
        saveVecData(
            subView._underlyingView,
            element,
            subView._region.startRow + row,
            subView._region.startCol + col);
    }


    template <typename View>
    size_t horisontalBorder(const SubView<View>& subView)
    {
        size_t underlyingBorder = horisontalBorder(subView._underlyingView);
        size_t result = 0;

        if (underlyingBorder > subView._region.startCol)
            result = std::max(result, underlyingBorder - subView._region.startCol);

        size_t leftDistance = getWidth(subView._underlyingView) - subView._region.endCol - 1;

        if (underlyingBorder > leftDistance)
            result = std::max(result, underlyingBorder - leftDistance);

        return result;
    }

    template <typename View>
    size_t verticalBorder(const SubView<View>& subView)
    {
        size_t underlyingBorder = verticalBorder(subView._underlyingView);
        size_t result = 0;

        if (underlyingBorder > subView._region.startRow)
            result = std::max(result, underlyingBorder - subView._region.startRow);

        size_t bottomDistance = getHeight(subView._underlyingView) - subView._region.endRow - 1;

        if (underlyingBorder > bottomDistance)
            result = std::max(result, underlyingBorder - bottomDistance);

        return result;
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
    /// <param name="startRow">Start row for sub-view.</param>
    /// <param name="endRow">End row for sub-view.</param>
    /// <param name="startCol">Start column for sub-view.</param>
    /// <param name="endCol">End column for sub-view.</param>
    template <typename View>
    auto sub_view(View&& view, size_t startRow, size_t endRow, size_t startCol, size_t endCol)
    {
        return __internal__::sub_view(std::forward<View>(view), __internal__::Region(startRow, endRow, startCol, endCol));
    }
}
