#pragma once
#include <vector>
#include <cassert>
#include "../dimensions.h"


namespace symd::__internal__
{
    template <typename T, std::size_t N>
    Dimensions getShape(const std::array<T, N>& x, typename UnderlyingRegister<T>::Type* = 0)
    {
        // Is enabled by UnderlyingRegister<T>::Type
        return Dimensions({ (int64_t) N });
    }

    template <typename T, std::size_t N>
    Dimensions getPitch(const std::array<T, N>& x, typename UnderlyingRegister<T>::Type* = 0)
    {
        return Dimensions({ 1 });
    }

    template <typename T, std::size_t N>
    T* getDataPtr(std::array<T, N>& x, const Dimensions& coords, typename UnderlyingRegister<T>::Type* = 0)
    {
        assert(coords.num_dims() == 1);
        assert(coords[0] < N);

        return x.data() + coords[0];
    }

    template <typename T, std::size_t N>
    const T* getDataPtr(const std::array<T, N>& x, const Dimensions& coords, typename UnderlyingRegister<T>::Type* = 0)
    {
        assert(coords.num_dims() == 1);
        assert(coords[0] < N);

        return x.data() + coords[0];
    }
}
