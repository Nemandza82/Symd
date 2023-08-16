#pragma once
#include "../dimensions.h"
#include <vector>


namespace symd::views
{
    /// <summary>
    /// Data view of underlying memory buffer. Can be passes to symd methods.
    /// </summary>
    template <typename T, int dim>
    class data_view
    {
        T* _data;
        Dimensions _shape;
        Dimensions _pitch;

    public:
        /// <summary>
        /// Contructs data_view of input memory buffer.
        /// </summary>
        /// <param name="ptr">Pointer to underlying memory buffer.</param>
        /// <param name="shape">Shape of underlying memory in elements (not bytes).</param>
        /// <param name="pitch">Pitch of data_view. Length of one lines in elements.</param>
        data_view(T* ptr, const Dimensions& shape, const Dimensions& pitch)
            : _shape(shape)
            , _pitch(pitch)
        {
            _data = ptr;
        }

        T* data()
        {
            return _data;
        }

        const T* data() const
        {
            return _data;
        }


        const Dimensions& shape() const
        {
            return _shape;
        }

        const Dimensions& pitch() const
        {
            return _pitch;
        }
    };

    template <typename T>
    data_view<T, 1> data_view_1d(T* ptr, int64_t length)
    {
        return data_view<T, 1>(ptr, Dimensions({ length }), Dimensions({ 1 }));
    }

    template <typename T>
    data_view<T, 2> data_view_2d(T* ptr, int64_t width, int64_t height, int64_t pitch)
    {
        return data_view<T, 2>(ptr, Dimensions({ height, width }), Dimensions({ pitch, 1 }));
    }
}

namespace symd::__internal__
{
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // Access the data
    /////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename T, int dim>
    Dimensions getShape(const views::data_view<T, dim>& dw)
    {
        return dw.shape();
    }

    template <typename T, int dim>
    Dimensions getPitch(const views::data_view<T, dim>& dw)
    {
        return dw.pitch();
    }

    template <typename T, int dim>
    T* getDataPtr(views::data_view<T, dim>& dw, const Dimensions& coords)
    {
        assert(coords.num_dims() == dim);

        T* dst = dw.data();
        auto pitch = dw.pitch();

        for (int i = 0; i < dim; i++)
        {
            assert(coords[i] < dw.shape()[i]);
            dst += coords[i] * pitch[i];
        }

        return dst;
    }

    template <typename T, int dim>
    const T* getDataPtr(const views::data_view<T, dim>& dw, const Dimensions& coords)
    {
        assert(coords.num_dims() == dim);

        const T* dst = dw.data();
        auto pitch = dw.pitch();

        for (int i = 0; i < dim; i++)
        {
            assert(coords[i] < dw.shape()[i]);
            dst += coords[i] * pitch[i];
        }

        return dst;
    }
}
