#pragma once
#include <vector>
#include <array>
#include <span>
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


    namespace __internal__
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
        size_t getPitch(const data_view<T, 1>& x)
        {
            return getWidth(x);
        }

        template <typename T>
        size_t getPitch(const data_view<T, 2>& x)
        {
            return getWidth(x.pitch);
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
            assert(row < dw.height);
            assert(col < dw.width);

            return dw.data + row * dw.pitch + col;
        }

        template <typename T>
        const T* getDataPtr(const data_view<T, 2>& dw, size_t row, size_t col)
        {
            assert(row < dw.height);
            assert(col < dw.width);

            return dw.data + row * dw.pitch + col;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////
        // Overloads for std::vector
        /////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T>
        size_t getWidth(const std::vector<T>& x)
        {
            return x.size();
        }

        template <typename T>
        size_t getHeight(const std::vector<T>& x)
        {
            return 1;
        }

        template <typename T>
        size_t getPitch(const std::vector<T>& x)
        {
            return getWidth(x);
        }

        template <typename T>
        T* getDataPtr(std::vector<T, std::allocator<T>>& x, size_t row, size_t col)
        {
            assert(row == 0);
            assert(col < x.size());

            return x.data() + col;
        }

        template <typename T>
        const T* getDataPtr(const std::vector<T, std::allocator<T>>& x, size_t row, size_t col)
        {
            assert(row == 0);
            assert(col < x.size());

            return x.data() + col;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////
        // Overloads for std::array
        /////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T, std::size_t N>
        size_t getWidth(const std::array<T, N>& x)
        {
            return N;
        }

        template <typename T, std::size_t N>
        size_t getHeight(const std::array<T, N>& x)
        {
            return 1;
        }

        template <typename T, std::size_t N>
        size_t getPitch(const std::array<T, N>& x)
        {
            return getWidth(x);
        }

        template <typename T, std::size_t N>
        T* getDataPtr(std::array<T, N>& x, size_t row, size_t col)
        {
            assert(row == 0);
            assert(col < x.size());

            return x.data() + col;
        }

        template <typename T, std::size_t N>
        const T* getDataPtr(const std::array<T, N>& x, size_t row, size_t col)
        {
            assert(row == 0);
            assert(col < x.size());

            return x.data() + col;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////
        // Overloads for std::span
        /////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T, std::size_t N>
        size_t getWidth(const std::span<T, N>& x)
        {
            return x.size();
        }

        template <typename T, std::size_t N>
        size_t getHeight(const std::span<T, N>& x)
        {
            return 1;
        }

        template <typename T, std::size_t N>
        size_t getPitch(const std::span<T, N>& x)
        {
            return getWidth(x);
        }

        template <typename T, std::size_t N>
        T* getDataPtr(std::span<T, N>& x, size_t row, size_t col)
        {
            assert(row == 0);
            assert(col < x.size());

            return x.data() + col;
        }

        template <typename T, std::size_t N>
        const T* getDataPtr(const std::span<T, N>& x, size_t row, size_t col)
        {
            assert(row == 0);
            assert(col < x.size());

            return x.data() + col;
        }


        ////////////////////////////////////////////////////////////////////////////////
        // Fetch and save methods
        ////////////////////////////////////////////////////////////////////////////////

        template <typename Input>
        auto fetchData(const Input& input, size_t row, size_t col)
        {
            auto ptr = getDataPtr(input, row, col);
            return *ptr;
        }

        template <typename Input>
        auto fetchVecData(const Input& input, size_t row, size_t col)
        {
            auto* ptr = getDataPtr(input, row, col);

            return SymdRegister<std::decay_t<decltype(*ptr)>>(ptr);
        }


        template <typename Output, typename X>
        auto saveData(Output& out, const X& x, size_t row, size_t col)
        {
            auto* ptr = getDataPtr(out, row, col);
            *ptr = x;
        }

        template <typename Output, typename X>
        auto saveVecData(Output& out, const SymdRegister<X>& x, size_t row, size_t col)
        {
            auto* ptr = getDataPtr(out, row, col);
            x.store(ptr);
        }
    }

    template<typename T>
    auto subView(T& view, const __internal__::Region& region)
    {
        auto* dataPtr = __internal__::getDataPtr(view, region.startRow, region.startCol);
        size_t pitch = __internal__::getPitch(view);

        return data_view<std::decay_t<decltype(*dataPtr)>, 2>(dataPtr, region.width(), region.height(), pitch);
    }
}
