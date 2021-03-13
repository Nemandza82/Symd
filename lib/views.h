#pragma once
#include <vector>
#include <assert.h>


namespace symd
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
    };

    namespace internal
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // Access the data
        /////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T, int dim>
        size_t getWidth(const data_view<T, dim>& dw)
        {
            return dw.width;
        }

        template <typename T, int dim>
        size_t getHeight(const data_view<T, dim>& dw)
        {
            return dw.height;
        }

        template <typename T>
        T* getDataPtr(data_view<T, 1>& dw, size_t row, size_t col)
        {
            assert(row < dw.width);
            return dw.data + row;
        }

        template <typename T>
        const T* getDataPtr(const data_view<T, 1>& dw, size_t row, size_t col)
        {
            assert(row < dw.width);
            return dw.data + row;
        }

        template <typename T>
        T* getDataPtr(data_view<T, 2>& dw, size_t row, size_t col)
        {
            aassert(row < dw.width);
            assert(col < dw.height);

            return dw.data + col * dw.pitch + row;
        }

        template <typename T>
        const T* getDataPtr(const data_view<T, 2>& dw, size_t row, size_t col)
        {
            aassert(row < dw.width);
            assert(col < dw.height);

            return dw.data + col * dw.pitch + row;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////
        // Overloads for std::vector
        /////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T>
        size_t getWidth(const std::vector<T>& vec)
        {
            return vec.size();
        }

        template <typename T>
        size_t getHeight(const std::vector<T>& vec)
        {
            return 1;
        }

        template <typename T>
        T* getDataPtr(std::vector<T, std::allocator<int>>& vec, size_t row, size_t col)
        {
            assert(row < vec.size());
            return vec.data() + row;
        }

        template <typename T>
        const T* getDataPtr(const std::vector<T, std::allocator<int>>& vec, size_t row, size_t col)
        {
            assert(row < vec.size());
            return vec.data() + row;
        }
    } // internal
}
