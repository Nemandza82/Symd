#pragma once
#include "symd_register.h"


namespace symd::__internal__
{
    template <typename View>
    struct Stencil
    {
        View _underlyingView;
        const int _width;
        const int _height;

        Stencil(View&& view, int width, int height)
            : _underlyingView(std::forward<View>(view))
            , _width(width)
            , _height(height)
        {}
    };

    static size_t foldCoords(__int64 x, size_t low, size_t high)
    {
        if (x < (__int64)low)
        {
            return size_t(2 * low + std::abs(x));
        }
        else if (x >= (__int64)high)
        {
            return size_t(2 * high - std::abs(x));
        }
        else
        {
            return size_t(x);
        }
    }

    /// <summary>
    /// Object to access stencil around specified data location (row, col)
    /// </summary>
    template <typename View>
    class StencilPix
    {
        const View& _underlyingView;
        const size_t _row;
        const size_t _col;

        size_t _underlyingWidth;
        size_t _underlyingHeight;

    public:
        using UnderlyingDataType = std::decay_t<decltype(fetchData(_underlyingView, 0, 0))>;

        StencilPix(const View& view, size_t row, size_t col)
            : _underlyingView(view)
            , _row(row)
            , _col(col)
        {
            _underlyingWidth = getWidth(_underlyingView);
            _underlyingHeight = getHeight(_underlyingView);
        }

        UnderlyingDataType operator()(int dr, int dc) const
        {
            size_t row = foldCoords((__int64)_row + dr, 0, (__int64)(_underlyingHeight - 1));
            size_t col = foldCoords((__int64)_col + dc, 0, (__int64)(_underlyingWidth - 1));

            return fetchData(_underlyingView, row, col);
        }
    };

    template <typename View>
    class StencilVec
    {
        const View& _underlyingView;
        const size_t _row;
        const size_t _col;

    public:
        using UnderlyingDataType = std::decay_t<decltype(fetchData(_underlyingView, 0, 0))>;

        StencilVec(const View& view, size_t row, size_t col)
            : _underlyingView(view)
            , _row(row)
            , _col(col)
        {
        }

        SymdRegister<UnderlyingDataType> operator()(int dr, int dc) const
        {
            return fetchVecData(_underlyingView, _row + dr, _col + dc);
        }
    };


    template <typename T>
    size_t getWidth(const Stencil<T>& x)
    {
        return getWidth(x._underlyingView);
    }

    template <typename T>
    size_t getHeight(const Stencil<T>& x)
    {
        return getHeight(x._underlyingView);
    }

    template <typename T>
    size_t getPitch(const Stencil<T>& x)
    {
        return getPitch(x._underlyingView);
    }

    template <typename T>
    auto fetchData(const Stencil<T>& x, size_t row, size_t col)
    {
        return StencilPix(x._underlyingView, row, col);
    }

    template <typename View>
    auto fetchVecData(const Stencil<View>& st, size_t row, size_t col)
    {
        return StencilVec(st._underlyingView, row, col);
    }

    template <typename View>
    size_t horisontalBorder(const Stencil<View>& st)
    {
        return (st._width / 2) + horisontalBorder(st._underlyingView);
    }

    template <typename View>
    size_t verticalBorder(const Stencil<View>& st)
    {
        return (st._height / 2) + verticalBorder(st._underlyingView);
    }
}


namespace symd::views
{
    template <typename View>
    auto stencil(View&& view, int width, int height)
    {
        return __internal__::Stencil<View>(std::forward<View>(view), width, height);
    }

    template<typename View>
    auto sub_view(const __internal__::Stencil<View>& st, const __internal__::Region& region)
    {
        return stencil(sub_view(st._underlyingView, region), st._width, st._height);
    }
}
