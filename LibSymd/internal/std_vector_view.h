#pragma once
#include <vector>
#include <cassert>
#include "../dimensions.h"


namespace symd::__internal__
{
    /// <summary>
    /// Gets the width of vector with fundamental data types.
    /// </summary>
    template <typename T>
    Dimensions getShape(const std::vector<T>& x, typename UnderlyingRegister<T>::Type* = 0)
    {
        return Dimensions({ (int64_t) x.size() });
    }

    /// <summary>
    /// Gets the pitch of vector with fundamental data types.
    /// </summary>
    template <typename T>
    Dimensions getPitch(const std::vector<T>& x, typename UnderlyingRegister<T>::Type* = 0)
    {
        return Dimensions({ 1 });
    }

    template <typename T>
    T* getDataPtr(std::vector<T, std::allocator<T>>& x, const Dimensions& coords, typename UnderlyingRegister<T>::Type* = 0)
    {
        assert(coords.num_dims() == 1);
        assert(coords[0] < x.size());

        return x.data() + coords[0];
    }

    template <typename T>
    const T* getDataPtr(const std::vector<T, std::allocator<T>>& x, const Dimensions& coords, typename UnderlyingRegister<T>::Type* = 0)
    {
        assert(coords.num_dims() == 1);
        assert(coords[0] < x.size());

        return x.data() + coords[0];
    }
}
