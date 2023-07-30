#pragma once
#include <vector>
#include <cassert>
#include "../dimensions.h"


namespace symd::__internal__
{
    template <typename T, std::size_t N>
    Dimensions getShape(const std::array<T, N>& x, typename std::enable_if<std::is_fundamental<T>::value, T>::type* = 0)
    {
        return Dimensions({ (int64_t) N });
    }

    template <typename T, std::size_t N>
    Dimensions getPitch(const std::array<T, N>& x, typename std::enable_if<std::is_fundamental<T>::value, T>::type* = 0)
    {
        return Dimensions({ 1 });
    }

    template <typename T, std::size_t N>
    T* getDataPtr(std::array<T, N>& x, const Dimensions& coords, typename std::enable_if<std::is_fundamental<T>::value, T>::type* = 0)
    {
        assert(coords.num_dims() == 1);
        assert(coords[0] < N);

        return x.data() + coords[0];
    }

    template <typename T, std::size_t N>
    const T* getDataPtr(const std::array<T, N>& x, const Dimensions& coords, typename std::enable_if<std::is_fundamental<T>::value, T>::type* = 0)
    {
        assert(coords.num_dims() == 1);
        assert(coords[0] < N);

        return x.data() + coords[0];
    }
}
