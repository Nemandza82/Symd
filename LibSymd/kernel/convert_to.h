
#pragma once
#include "../internal/symd_register.h"


namespace symd::kernel
{
    /// <summary>
    /// Converts data from one scalar format to another (float -> int).
    /// </summary>
    template <typename R, typename T>
    R convert_to(T x)
    {
        // This overload is required to comply to SIMD standard to clamp when converting...
        static_assert(__internal__::UnderlyingRegister<T>::is_supported_type(), "Unsupported type.");
        static_assert(__internal__::UnderlyingRegister<R>::is_supported_type(), "Unsupported type.");
        
        if constexpr (std::is_same_v<R, symd::bfloat16>)
        {
            // Conversion to bfloat16 first convert to float
            return symd::bfloat16(convert_to<float>(x));
        }
        else if constexpr (std::is_same_v<T, symd::bfloat16>)
        {
            // Convertions from bfloat16 to R -> First convert x to float than to R
            return convert_to<R>((float)x);
        }
        else
        {
            // When converting to integral types check limits
            if constexpr (std::is_same_v<R, int> || std::is_same_v<R, unsigned char>)
            {
                if (x > std::numeric_limits<R>::max())
                    return std::numeric_limits<R>::max();

                if (x < std::numeric_limits<R>::min())
                    return std::numeric_limits<R>::min();
            }

            return (R)x;
        }
    }

    /// <summary>
    /// Converts data in vector register from one scalar format to another (float -> int)
    /// Returns new register with converted data.
    /// </summary>
    template <typename R, typename T>
    __internal__::SymdRegister<R> convert_to(const __internal__::SymdRegister<T>& in)
    {
        static_assert(__internal__::UnderlyingRegister<T>::is_supported_type(), "Unsupported type.");
        static_assert(__internal__::UnderlyingRegister<R>::is_supported_type(), "Unsupported type.");

        return in.template convert_to<R>();
    }
} // kernel
