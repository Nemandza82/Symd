#pragma once
#include "basic_views.h"
#include "../dimensions.h"


namespace symd
{
    /// <summary>
    /// Specifies how accesses outside of underlying view are hendled.
    /// </summary>
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
        Dimensions _border;

        Border _borderHandling;
        C _borderConstant;

        Stencil(View&& view, const Dimensions& borderSize, Border borderHandling = Border::mirror, C borderConstant = C(0))
            : _underlyingView(std::forward<View>(view))
            , _border(borderSize)
        {
            _borderHandling = borderHandling;
            _borderConstant = borderConstant;
        }
    };

    /// <summary>
    /// Object to access stencil around specified data location (row, col)
    /// </summary>
    template <typename View, typename C>
    class StencilPix
    {
        const View& _underlyingView;
        const Dimensions& _coords;

        Dimensions _underlyingShape;

        Border _borderHandling;
        C _borderConstant;

    public:
        using UnderlyingDataType = std::decay_t<decltype(fetchData(_underlyingView, _coords))>;

        StencilPix(const View& view, const Dimensions& coords, Border borderHandling, C borderConstant)
            : _underlyingView(view)
            , _coords(coords)
        {
            _underlyingShape = getShape(_underlyingView);
            _borderHandling = borderHandling;            
            _borderConstant = borderConstant;
        }

        UnderlyingDataType operator()(int64_t d0) const
        {
            assert(_coords.num_dims() == 1);
            return handleBorders(_coords.add(d0));
        }

        UnderlyingDataType operator()(int64_t d0, int64_t d1) const
        {
            assert(_coords.num_dims() == 2);
            return handleBorders(_coords.add(d0, d1));
        }

        UnderlyingDataType operator()(int64_t d0, int64_t d1, int64_t d2) const
        {
            assert(_coords.num_dims() == 3);
            return handleBorders(_coords.add(d0, d1, d2));
        }

        UnderlyingDataType operator()(int64_t d0, int64_t d1, int64_t d2, int64_t d3) const
        {
            assert(_coords.num_dims() == 4);
            return handleBorders(_coords.add(d0, d1, d2, d3));
        }

        UnderlyingDataType operator()(int64_t d0, int64_t d1, int64_t d2, int64_t d3, int64_t d4) const
        {
            assert(_coords.num_dims() == 5);
            return handleBorders(_coords.add(d0, d1, d2, d3, d4));
        }

        UnderlyingDataType handleBorders(const Dimensions& coords) const
        {
            switch (_borderHandling)
            {
                case Border::constant:
                    {
                        if (_underlyingShape.are_outside(coords))
                            return _borderConstant;
                        else 
                            return fetchData(_underlyingView, coords);
                    }
                case Border::mirror:
                    {
                        return fetchData(_underlyingView, _underlyingShape.mirrorCoords(coords));
                    }
                case Border::replicate:
                    {
                        return fetchData(_underlyingView, _underlyingShape.replicateCoords(coords));
                    }
                case Border::mirror_replicate:
                    {
                        return fetchData(_underlyingView, _underlyingShape.replicateMirrorCoords(coords));
                    }
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
        const Dimensions& _coords;

    public:
        using UnderlyingDataType = std::decay_t<decltype(fetchData(_underlyingView, _coords))>;

        StencilVec(const View& view, const Dimensions& coords)
            : _underlyingView(view)
            , _coords(coords)
        {
        }

        SymdRegister<UnderlyingDataType> operator()(int64_t d0) const
        {
            assert(_coords.num_dims() == 1);
            return fetchVecData(_underlyingView, _coords.add(d0));
        }

        SymdRegister<UnderlyingDataType> operator()(int64_t d0, int64_t d1) const
        {
            assert(_coords.num_dims() == 2);
            return fetchVecData(_underlyingView, _coords.add(d0, d1));
        }

        SymdRegister<UnderlyingDataType> operator()(int64_t d0, int64_t d1, int64_t d2) const
        {
            assert(_coords.num_dims() == 3);
            return fetchVecData(_underlyingView, _coords.add(d0, d1, d2));
        }

        SymdRegister<UnderlyingDataType> operator()(int64_t d0, int64_t d1, int64_t d2, int64_t d3) const
        {
            assert(_coords.num_dims() == 4);
            return fetchVecData(_underlyingView, _coords.add(d0, d1, d2, d3));
        }

        SymdRegister<UnderlyingDataType> operator()(int64_t d0, int64_t d1, int64_t d2, int64_t d3, int64_t d4) const
        {
            assert(_coords.num_dims() == 5);
            return fetchVecData(_underlyingView, _coords.add(d0, d1, d2, d3, d4));
        }
    };

    template <typename View, typename C>
    Dimensions getShape(const Stencil<View, C>& x)
    {
        return getShape(x._underlyingView);
    }

    template <typename View, typename C>
    size_t getPitch(const Stencil<View, C>& x)
    {
        return getPitch(x._underlyingView);
    }

    template <typename View, typename C>
    auto fetchData(const Stencil<View, C>& x, const Dimensions& coords)
    {
        return StencilPix(x._underlyingView, coords, x._borderHandling, x._borderConstant);
    }

    template <typename View, typename C>
    auto fetchVecData(const Stencil<View, C>& st, const Dimensions& coords)
    {
        return StencilVec(st._underlyingView, coords);
    }

    template <typename View, typename C>
    Dimensions getBorder(const Stencil<View, C>& st)
    {
        return st._border + getBorder(st._underlyingView);
    }
}

namespace symd::views
{
    /// <summary>
    /// Creates stencil view from input view, so you can access nearby elements inside kernel.
    /// </summary>
    /// <param name="view">Underlying view.</param>
    /// <param name="borders">borders of the stencil window.</param>
    template <typename View>
    auto stencil(View&& view, const Dimensions& borders)
    {
        return __internal__::Stencil<View, int>(std::forward<View>(view), borders, Border::mirror, 0);
    }

    /// <summary>
    /// Creates stencil view from input view, so you can access nearby elements inside kernel.
    /// </summary>
    /// <param name="view">Underlying view.</param>
    /// <param name="borders">borders of the stencil window.</param>
    /// <param name="borderHandling">Specify how accesses outside of underlying view are handled. Can be constant, replicate, mirror...</param>
    template <typename View>
    auto stencil(View&& view, const Dimensions& borders, Border borderHandling)
    {
        return __internal__::Stencil<View, int>(std::forward<View>(view), borders, borderHandling, 0);
    }

    /// <summary>
    /// Creates stencil view from input view, so you can access nearby elements inside kernel.
    /// </summary>
    /// <param name="view">Underlying view.</param>
    /// <param name="width">Width of the stencil window.</param>
    /// <param name="height">Height of the stencil window.</param>
    /// <param name="borderHandling">Specify how accesses outside of underlying view are handled. Can be constant, replicate, mirror...</param>
    /// <param name="borderConstant">Constant that replaces value when kernel accesses ourside of underlying view.</param>
    template <typename View, typename C>
    auto stencil(View&& view, const Dimensions& borders, Border borderHandling, C borderConstant)
    {
        return __internal__::Stencil<View, C>(std::forward<View>(view), borders, borderHandling, borderConstant);
    }
}

namespace symd::__internal__
{
    /// <summary>
    /// Creates subview for underlying Stencil view.
    /// </summary>
    template<typename View, typename C>
    auto sub_view(const Stencil<View, C>& st, const Region& region)
    {
        return stencil(sub_view(st._underlyingView, region), st._borders);
    }
}
