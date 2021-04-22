#pragma once
#include <vector>
#include <array>
///#include <span>
#include <assert.h>
#include "data_view.h"
#include "symd_register.h"
#include "region.h"


namespace symd::__internal__
{
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

    /*template <typename T, std::size_t N>
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
    }*/


    ////////////////////////////////////////////////////////////////////////////////
    // Fetch and save methods
    ////////////////////////////////////////////////////////////////////////////////

    template <typename View>
    auto fetchData(const View& view, size_t row, size_t col)
    {
        auto ptr = getDataPtr(view, row, col);
        return *ptr;
    }

    template <typename View>
    auto fetchVecData(const View& view, size_t row, size_t col)
    {
        auto* ptr = getDataPtr(view, row, col);

        return SymdRegister<std::decay_t<decltype(*ptr)>>(ptr);
    }


    template <typename View, typename DataType>
    void saveData(View& outView, const DataType& element, size_t row, size_t col)
    {
        auto* ptr = getDataPtr(outView, row, col);
        *ptr = element;
    }

    template <typename View, typename DataType>
    void saveVecData(View& outView, const SymdRegister<DataType>& element, size_t row, size_t col)
    {
        auto* ptr = getDataPtr(outView, row, col);
        element.store(ptr);
    }

    template <typename View>
    size_t horisontalBorder(const View& input)
    {
        return 0;
    }

    template <typename View>
    size_t verticalBorder(const View& input)
    {
        return 0;
    }
}
