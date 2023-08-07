#pragma once
#include "../internal/symd_register.h"


namespace symd::kernel
{
    // log(x) = log(a*2^n) = log(a) + n*log(2) = log(1 + z) + n*log(2)
    // log(2) = 0.30102999566
    // a in range [0, 1]
    // z in range [-1, 0]
    // log(1 + z) = z - z^2/2 + z^3/3
    template <typename T> 
    T log(T x)
    {
        auto two_to_n = fp_2_pow_exp(x);
        auto n = fp_exp(x);
        auto a = x / two_to_n;
        auto z = a - 1;

        auto z2 = z * z;
        auto z3 = z2 * z;
        auto z4 = z2 * z2;
        auto z5 = z4 * z;

        auto log_1_plus_z = z 
            - (z2 / 2)
            + (z3 / 3)
            - (z4 / 4) 
            + (z4 * z) / 5
            - (z4 * z2) / 6
            + (z4 * z3) / 7
            - (z4 * z4) / 8
            + (z5 * z4) / 9
            - (z5 * z5) / 10;

        auto result = log_1_plus_z + convert_to<float>(n) * 0.69314718056f;
        return blend(x == 1.0f, 0.0f, result);
    }
}
