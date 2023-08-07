#pragma once
#include "../internal/symd_register.h"
#include <cmath>


namespace symd::kernel
{
    // http://spfrnd.de/posts/2018-03-10-fast-exponential.html
    namespace __internal_exp
    {
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
    }


    int fp_exp(float x)
    {
        unsigned int as_int = *((unsigned int*)(&x));
        return ((as_int << 1) >> 24) - 126;
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
        as_int = (as_int >> 23) + 1;
        as_int = as_int << 23;

        return *((float*)(&as_int));
    }

    __internal__::SymdRegister<float> fp_2_pow_exp(const __internal__::SymdRegister<float>& x)
    {
        return blend(x == 0.0f, __internal__::SymdRegister<float>(0.0f), x.fp_2_pow_exp());
    }
    
    template <typename T>
    T exp(T x)
    {
        constexpr int N = 32 * 1024;
        return __internal_exp::pow_int<T, N>(1.0f + x / (float)N);
    }
}
