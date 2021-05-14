#pragma once
#include "symd_register.h"

namespace symd
{
    enum class Border
    {
        constant,
        replicate,
        mirror,
        mirror_replicate
    };
}

namespace symd::__internal__
{
    template <typename View, typename C>
    struct Stencil
    {
        View _underlyingView;
        const int _width;
        const int _height;

        Border _borderHandling;
        C _borderConstant;

        Stencil(View&& view, int width, int height, Border borderHandling = Border::mirror, C c = C(0))
            : _underlyingView(std::forward<View>(view))
            , _width(width)
            , _height(height)
        {
            _borderHandling = borderHandling;
            _borderConstant = c;
        }
    };

    static size_t mirrorCoords(int64_t x, size_t low, size_t high)
    {
        if (x < (int64_t)low)
        {
            return size_t(2 * low + std::abs(x));
        }
        else if (x >= (int64_t)high)
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
    template <typename View, typename C>
    class StencilPix
    {
        const View& _underlyingView;
        const size_t _row;
        const size_t _col;

        size_t _underlyingWidth;
        size_t _underlyingHeight;

        Border _borderHandling;
        C _borderConstant;

    public:
        using UnderlyingDataType = std::decay_t<decltype(fetchData(_underlyingView, 0, 0))>;

        StencilPix(const View& view, size_t row, size_t col, Border borderHandling, C c)
            : _underlyingView(view)
            , _row(row)
            , _col(col)
        {
            _underlyingWidth = getWidth(_underlyingView);
            _underlyingHeight = getHeight(_underlyingView);

            _borderHandling = borderHandling;            
            _borderConstant = c;
        }

        UnderlyingDataType operator()(int dr, int dc) const
        {
            const auto resRow = (int64_t)_row + dr;
            const auto resCol = (int64_t)_col + dc;

            // TODO: First cast then sub?
            const auto heightLimit = (int64_t)(_underlyingHeight - 1);
            const auto widthLimit = (int64_t)(_underlyingWidth - 1);

            switch (_borderHandling)
            {
                case Border::mirror:
                    {
                        size_t finalRow = mirrorCoords(resRow, 0, heightLimit);
                        size_t finalCol = mirrorCoords(resCol, 0, widthLimit);

                        return fetchData(_underlyingView, finalRow, finalCol);
                    }
                case Border::constant:
                    {
                        if (resRow < 0 || resRow > heightLimit || resCol < 0 || resCol > widthLimit)
                            return _borderConstant;
                        else 
                            return fetchData(_underlyingView, resRow, resCol);
                    }
                case Border::replicate:
                    // TODO
                case Border::mirror_replicate:
                    // TODO
                default:
                    // Can't happen
                    return UnderlyingDataType();
            }
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


    template <typename View, typename C>
    size_t getWidth(const Stencil<View, C>& x)
    {
        return getWidth(x._underlyingView);
    }

    template <typename View, typename C>
    size_t getHeight(const Stencil<View, C>& x)
    {
        return getHeight(x._underlyingView);
    }

    template <typename View, typename C>
    size_t getPitch(const Stencil<View, C>& x)
    {
        return getPitch(x._underlyingView);
    }

    template <typename View, typename C>
    auto fetchData(const Stencil<View, C>& x, size_t row, size_t col)
    {
        return StencilPix(x._underlyingView, row, col, x._borderHandling, x._borderConstant);
    }

    template <typename View, typename C>
    auto fetchVecData(const Stencil<View, C>& st, size_t row, size_t col)
    {
        return StencilVec(st._underlyingView, row, col);
    }

    template <typename View, typename C>
    size_t horisontalBorder(const Stencil<View, C>& st)
    {
        return (st._width / 2) + horisontalBorder(st._underlyingView);
    }

    template <typename View, typename C>
    size_t verticalBorder(const Stencil<View, C>& st)
    {
        return (st._height / 2) + verticalBorder(st._underlyingView);
    }
}


namespace symd::views
{
    template <typename View>
    auto stencil(View&& view, int width, int height)
    {
        return __internal__::Stencil<View, int>(std::forward<View>(view), width, height, Border::mirror, 0);
    }

    template <typename View>
    auto stencil(View&& view, int width, int height, Border borderHandling)
    {
        return __internal__::Stencil<View, int>(std::forward<View>(view), width, height, borderHandling, 0);
    }

    template <typename View, typename C>
    auto stencil(View&& view, int width, int height, Border borderHandling, C c)
    {
        return __internal__::Stencil<View, C>(std::forward<View>(view), width, height, borderHandling, c);
    }
}

namespace symd::__internal__
{
    template<typename View, typename C>
    auto sub_view(const Stencil<View, C>& st, const Region& region)
    {
        return stencil(sub_view(st._underlyingView, region), st._width, st._height);
    }
}
