#pragma once
#include "../internal/symd_register.h"
#include <cmath>


namespace symd::kernel
{
    int fp_exp(float x)
    {
        unsigned int as_int = *((unsigned int*)(&x));
        return ((as_int << 1) >> 24) - 127;
    }

    __internal__::SymdRegister<int> fp_exp(const __internal__::SymdRegister<float>& x)
    {
        return x.fp_exp();
    }

    float fp_2_pow_exp(float x)
    {
        if (x == 0)
            return 0;

        unsigned int as_int = *((unsigned int*)(&x));
        as_int = (as_int >> 23);
        as_int = as_int << 23;

        return *((float*)(&as_int));
    }

    __internal__::SymdRegister<float> fp_2_pow_exp(const __internal__::SymdRegister<float>& x)
    {
        return blend(x == 0.0f, __internal__::SymdRegister<float>(0.0f), x.fp_2_pow_exp());
    }
    
    namespace __internal_exp
    { 
        // http://spfrnd.de/posts/2018-03-10-fast-exponential.html
        template <typename T, int N>
        T pow_int(T x)
        {
            if constexpr (N == 1)
            {
                return x;
            }
            else
            {
                auto y = pow_int<T, N/2>(x);
                return y * y;
            }
        }

        // exp(x) = lim (1 + x /n)^n
        template <typename T>
        T exp_limes(T x)
        {
            constexpr int N = 32 * 1024;
            return __internal_exp::pow_int<T, N>(1.0f + x / (float)N);
        }

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

    template <typename T>
    T exp(T x)
    {
        // e^x = 2^(x / 0.69314718056f)
        return __internal_exp::fastpow2f(x / 0.69314718056);

        //return __internal_exp::exp_limes(x);
    }
}
