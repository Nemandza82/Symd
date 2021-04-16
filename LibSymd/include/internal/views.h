#pragma once
#include <vector>
#include <array>
#include <span>
#include <assert.h>
#include "internal/data_view.h"


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

namespace symd::views
{
    template<typename View>
    auto sub_view(View& view, const __internal__::Region& region)
    {
        auto* dataPtr = __internal__::getDataPtr(view, region.startRow, region.startCol);
        size_t pitch = __internal__::getPitch(view);

        return data_view<std::decay_t<decltype(*dataPtr)>, 2>(dataPtr, region.width(), region.height(), pitch);
    }
}
