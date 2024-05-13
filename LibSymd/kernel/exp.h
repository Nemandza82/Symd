#pragma once
#include "../internal/symd_register.h"
#include <cmath>


namespace symd::kernel
{
    // Function to extract the exponent from a single-precision floating-point number in IEEE 754 format.
    int fp_exp(float x)
    {
        // Reinterpret the float bits as an unsigned int
        unsigned int as_int = *((unsigned int*)(&x));

        // Extract the exponent bits (bits 23 to 30) by shifting left then right
        return ((as_int << 1) >> 24) - 127;
    }

    __internal__::SymdRegister<int> fp_exp(const __internal__::SymdRegister<float>& x)
    {
        return x.fp_exp();
    }

    // Returns a new float with the same exponent as the input float, but with a significant part of zero.
    // This effectively extracts the exponent part of the input float.
    float exp_part_of_float(float x)
    {
        if (x == 0)
            return 0;

        // Interpret the memory location of the float as an unsigned integer to manipulate its bits directly.
        unsigned int as_int = *((unsigned int*)(&x));

        // Shift the bits of the integer 23 places to the right to remove the significant (mantissa) part of the float.
        as_int = (as_int >> 23);

        // Shift the bits of the integer 23 places back to the left to put the exponent and sign bit back in their original position.
        as_int = as_int << 23;

        // Interpret the integer as a float again and return this value.
        return *((float*)(&as_int));
    }

    __internal__::SymdRegister<float> exp_part_of_float(const __internal__::SymdRegister<float>& x)
    {
        return blend(x == 0.0f, __internal__::SymdRegister<float>(0.0f), x.exp_part_of_float());
    }
    
    namespace __internal_exp
    { 
        // http://spfrnd.de/posts/2018-03-10-fast-exponential.html
        template<typename DType, typename T>
        T exp_teylor(T x)
        {
            auto term = x;
        
            auto eValue = term + (DType)1.0;
            term = term * x * (DType)0.5;

            eValue += term;
            term = term * x * (DType)0.33333333333;

            eValue += term;
            term = term * x * (DType)0.25;

            eValue += term;
            term = term * x * (DType)0.2;

            eValue += term;
            term = term * x * (DType)0.16666666666;

            eValue += term;
            term = term * x * (DType)0.14285714285;

            // eValue += term;
            // term = term * x / 8;

            return eValue;
        }

        // Calculates 2^n with integer exponent by bit ops to set exponent field of float.
        template <typename T>
        T fastpow2(int n)
        {
            if constexpr (std::is_same_v<T, float>)
            {
                n = (n + 127) << 23; // Move exponnent to right place
                float as_float = *((float*)(&n));
                return as_float;
            }
            else if constexpr (std::is_same_v<T, symd::bfloat16>)
            {
                return fastpow2<float>(n);
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                int64_t n64 = n + 1023; // Add bias to exponent
                n64 = n64 << 52; // Move exponnent to right place
                return *((double*)(&n64));
            }
        }

        // Calculates 2^n with integer exponent by bit ops to set exponent field of float.
        template <typename T>
        __internal__::SymdRegister<T> fastpow2(const __internal__::SymdRegister<int>& n)
        {
            return __internal__::SymdRegister<T>::fastpow2(n);
        }

        template<typename DType, typename T>
        T fastpow2f_impl(T x)
        {
            // Convert to int to calculate integer part of input x
            auto n = convert_to<int>(x);
            
            // Calculate difference 
            auto diff = x - convert_to<DType>(n);

            // 2^x = 2^(n+diff) = 2^n * 2^diff = 2^n * e^(diff * 0.69314718056)
            return fastpow2<DType>(n) * exp_teylor<DType>(diff * (DType)0.69314718056);
        }

        template<typename T>
        T fastpow2f(T x)
        {
            return fastpow2f_impl<T, T>(x);
        }

        template <typename T>
        __internal__::SymdRegister<T> fastpow2f(const __internal__::SymdRegister<T>& x)
        {
            return fastpow2f_impl<T, __internal__::SymdRegister<T>>(x);
        }
    }


    /// Computes 2 to the power of x.
    /// Very fast implementation is acheived by manipulating bits of IEEE float 754 representation.
    template <typename T>
    T pow2(T x)
    {
        return __internal_exp::fastpow2f(x);
    }


    /// Computes e (Euler's number) raised to the power of x.
    template <typename T>
    T exp(T x)
    {
        // e^x = 2^(x / 0.69314718056f)
        return __internal_exp::fastpow2f(x / (T)0.69314718056);
    }
}
