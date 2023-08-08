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

        template <typename T>
        T exp_teylor(T x)
        {
            auto term = x;
        
            auto eValue = term + 1.0f;
            term = term * x * 0.5f;

            eValue += term;
            term = term * x * 0.33333333333f;

            eValue += term;
            term = term * x * 0.25f;

            eValue += term;
            term = term * x * 0.2f;

            eValue += term;
            term = term * x * 0.16666666666f;

            eValue += term;
            term = term * x * 0.14285714285f;

            // eValue += term;
            // term = term * x / 8;

            return eValue;
        }

        // Calculates 2^n with integer exponent by bit ops to set exponent field of float.
        float fastpow2(int n)
        {
            n = (n + 127) << 23; // Move exponnent to right place
            float as_float = *((float*)(&n));
            return as_float;
        }

        // Calculates 2^n with integer exponent by bit ops to set exponent field of float.
        __internal__::SymdRegister<float> fastpow2(const __internal__::SymdRegister<int>& n)
        {
            return __internal__::SymdRegister<float>::fastpow2(n);
        }

        template<typename T>
        T fastpow2f(T x)
        {
            // Convert to int to calculate integer part of input x
            auto n = convert_to<int>(x);
            
            // Calculate difference 
            auto diff = x - convert_to<float>(n);

            // 2^x = 2^(n+diff) = 2^n * 2^diff = 2^n * e^(diff * 0.69314718056)
            return fastpow2(n) * exp_teylor(diff * 0.69314718056f);
        }
    }

    template <typename T>
    T exp(T x)
    {
        // e^x = 2^(x / 0.69314718056f)
        return __internal_exp::fastpow2f(x / 0.69314718056f);

        //return __internal_exp::exp_limes(x);
    }
}
