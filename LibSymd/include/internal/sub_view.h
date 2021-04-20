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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handle multiple outputs to sub-veiew
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename View, typename DataType, int count>
    void saveVecData(SubView<View>& subView, const std::array<SymdRegister<DataType>, count>& element, size_t row, size_t col)
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
}


namespace symd::views
{
    template <typename View>
    auto sub_view(View&& view, const __internal__::Region& region)
    {
        return __internal__::SubView<View>(std::forward<View>(view), region);
    }
}
