
#pragma once
#include "../internal/symd_register.h"


namespace symd::kernel
{
    /// <summary>
    /// According to selector blends first and second parameters. Similar to ternary operator.
    /// </summary>
    template <typename T>
    inline __internal__::SymdRegister<T> blend(
        const __internal__::SymdRegister<T>& selector,
        const __internal__::SymdRegister<T>& first,
        const __internal__::SymdRegister<T>& second)
    {
        static_assert(__internal__::UnderlyingRegister<T>::is_supported_type(), "Unsupported type.");
        return selector.blend(first, second);
    }


    template <typename T>
    inline __internal__::SymdRegister<T> blend(
        const __internal__::SymdRegister<T>& selector,
        const __internal__::SymdRegister<T>& first,
        const T& second)
    {
        static_assert(__internal__::UnderlyingRegister<T>::is_supported_type(), "Unsupported type.");
        return selector.blend(first, __internal__::SymdRegister<T>(second));
    }


    template <typename T>
    inline __internal__::SymdRegister<T> blend(
        const __internal__::SymdRegister<T>& selector,
        const T& first,
        const __internal__::SymdRegister<T>& second)
    {
        static_assert(__internal__::UnderlyingRegister<T>::is_supported_type(), "Unsupported type.");
        return selector.blend(__internal__::SymdRegister<T>(first), second);
    }


    /// <summary>
    /// According to selector chooses first or second parameters (ternary oprator).
    /// </summary>
    template <typename T>
    inline T blend(bool selector, const T& first, const T& second)
    {
        static_assert(__internal__::UnderlyingRegister<T>::is_supported_type(), "Unsupported type.");
        return selector ? first : second;
    }
} // kernel
