#pragma once
#include "../dimensions.h"


namespace symd::views
{
    template <typename T, int dim>
    class data_view
    {
    };

    /// <summary>
    /// 1d view of underlying memory buffer. Can be passes to symd methods.
    /// </summary>
    template <typename T>
    struct data_view<T, 1>
    {
        T* data;
        int64_t width;
        int64_t height;

        /// <summary>
        /// Contructs 1D data_view of input memory buffer.
        /// </summary>
        /// <param name="ptr">Pointer to underlying memory buffer.</param>
        /// <param name="length_">Pointer to underlying memory in elements (not bytes).</param>
        data_view(T* ptr, int64_t length_)
        {
            data = ptr;
            width = length_;
            height = 1;
        }
    };

    /// <summary>
    /// 2d view of underlying memory buffer. Can be passes to symd methods.
    /// </summary>
    template <typename T>
    struct data_view<T, 2>
    {
        T* data;

        int64_t width;
        int64_t height;
        int64_t pitch;

        /// <summary>
        /// Contructs 1D data_view of input memory buffer.
        /// </summary>
        /// <param name="ptr">Pointer to underlying memory buffer.</param>
        /// <param name="width_">Width of data_view.</param>
        /// <param name="height_">Width of data_view.</param>
        /// <param name="pitch_">Pitch of data_view. Length of one line in elements.</param>
        data_view(T* ptr, int64_t width_, int64_t height_, int64_t pitch_)
        {
            data = ptr;
            width = width_;
            height = height_;
            pitch = pitch_;
        }

        T readPix(int64_t y, int64_t x) const
        {
            return data[y * pitch + x];
        }
    };
}

namespace symd::__internal__
{
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // Access the data
    /////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    Dimensions getShape(const views::data_view<T, 1>& dw)
    {
        return Dimensions({ dw.width });
    }

    template <typename T>
    Dimensions getShape(const views::data_view<T, 2>& dw)
    {
        return Dimensions({ dw.height, dw.width });
    }

    template <typename T>
    Dimensions getPitch(const views::data_view<T, 1>& dw)
    {
        return Dimensions({ 1 });
    }

    template <typename T>
    Dimensions getPitch(const views::data_view<T, 2>& dw)
    {
        return Dimensions({ dw.pitch, 1 });
    }

    template <typename T>
    T* getDataPtr(views::data_view<T, 1>& dw, const Dimensions& coords)
    {
        assert(coords.count() == 1)
        assert(coords[0] < dw.width);

        return dw.data + coords[0];
    }

    template <typename T>
    const T* getDataPtr(const views::data_view<T, 1>& dw, const Dimensions& coords)
    {
        assert(coords.count() == 1)
        assert(coords[0] < dw.width);

        return dw.data + coords[0];
    }

    template <typename T>
    T* getDataPtr(views::data_view<T, 2>& dw, const Dimensions& coords)
    {
        assert(coords.count() == 2)
        assert(coords[0] < dw.height);
        assert(coords[1] < dw.width);

        return dw.data + coords[0] * dw.pitch + coords[1];
    }

    template <typename T>
    const T* getDataPtr(const views::data_view<T, 2>& dw, const Dimensions& coords)
    {
        assert(coords.count() == 2)
        assert(coords[0] < dw.height);
        assert(coords[1] < dw.width);

        return dw.data + coords[0] * dw.pitch + coords[1];
    }
}
