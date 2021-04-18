#pragma once


namespace symd::views
{
    template <typename T, int dim>
    class data_view
    {
    };

    template <typename T>
    struct data_view<T, 1>
    {
        T* data;
        size_t width;
        size_t height;

        // Contructs 1D data_view
        data_view(T* ptr, size_t length_)
        {
            data = ptr;
            width = length_;
            height = 1;
        }
    };

    template <typename T>
    struct data_view<T, 2>
    {
        T* data;

        size_t width;
        size_t height;
        size_t pitch;

        // Contructs 2D data_view
        data_view(T* ptr, size_t width_, size_t height_, size_t pitch_)
        {
            data = ptr;
            width = width_;
            height = height_;
            pitch = pitch_;
        }

        T readPix(size_t y, size_t x) const
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

    template <typename T, int dim>
    size_t getWidth(const views::data_view<T, dim>& dw)
    {
        return dw.width;
    }

    template <typename T, int dim>
    size_t getHeight(const views::data_view<T, dim>& dw)
    {
        return dw.height;
    }

    template <typename T>
    size_t getPitch(const views::data_view<T, 1>& x)
    {
        return getWidth(x);
    }

    template <typename T>
    size_t getPitch(const views::data_view<T, 2>& x)
    {
        return x.pitch;
    }

    template <typename T>
    T* getDataPtr(views::data_view<T, 1>& dw, size_t row, size_t col)
    {
        assert(row < dw.width);
        return dw.data + row;
    }

    template <typename T>
    const T* getDataPtr(const views::data_view<T, 1>& dw, size_t row, size_t col)
    {
        assert(row < dw.width);
        return dw.data + row;
    }

    template <typename T>
    T* getDataPtr(views::data_view<T, 2>& dw, size_t row, size_t col)
    {
        assert(row < dw.height);
        assert(col < dw.width);

        return dw.data + row * dw.pitch + col;
    }

    template <typename T>
    const T* getDataPtr(const views::data_view<T, 2>& dw, size_t row, size_t col)
    {
        assert(row < dw.height);
        assert(col < dw.width);

        return dw.data + row * dw.pitch + col;
    }
}

