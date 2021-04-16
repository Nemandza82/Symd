#pragma once
#include "symd_register.h"


namespace symd::views
{
    template <typename View>
    struct stencil
    {
        const View& _underlyingView;
        const int _stencilWidth;
        const int _stencilHeight;

        stencil(const View& view, int stencilWidth, int stencilHeight)
            : _underlyingView(view)
            , _stencilWidth(stencilWidth)
            , _stencilHeight(stencilHeight)
        {}
    };
}

namespace symd::__internal__
{
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

        size_t foldCoords(__int64 x, size_t low, size_t high)
        {
            if (x < low)
            {
                return size_t(2 * low + std::abs(x));
            }
            else if (x >= high)
            {
                return size_t(2 * high - std::abs(x));
            }
            else
            {
                return size_t(x);
            }
        }

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

        StencilVec(const View& view, int row, int col)
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
    size_t getWidth(const symd::views::stencil<T>& x)
    {
        return getWidth(x._underlying);
    }

    template <typename T>
    size_t getHeight(const symd::views::stencil<T>& x)
    {
        return getHeight(x._underlying);
    }

    template <typename T>
    size_t getPitch(const symd::views::stencil<T>& x)
    {
        return getPitch(x._underlying);
    }

    template <typename T>
    auto fetchData(const symd::views::stencil<T>& x, size_t row, size_t col)
    {
        return StencilPix(x._underlyingView, row, col);
    }

    template <typename View>
    auto fetchVecData(const symd::views::stencil<View>& st, size_t row, size_t col)
    {
        return StencilVec(st._underlyingView, row, col);
    }
}
