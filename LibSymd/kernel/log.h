#pragma once
#include "../internal/symd_register.h"


namespace symd::kernel
{
    // log(x) = log(a*2^n) = log(a) + n*log(2) = log(1 + z) + n*log(2)
    // log(2) = 0.30102999566
    // a in range [0, 1]
    // z in range [-1, 0]
    // log(1 + z) = z - z^2/2 + z^3/3 ...
    template <typename T> 
    T log(T x)
    {
        auto two_to_n = fp_2_pow_exp(x);
        auto n = fp_exp(x);
        auto a = x / two_to_n;
        auto z = a - 1;

        auto zp = z;
        auto log_1_plus_z = zp;

        zp *= z;
        log_1_plus_z -= zp * 0.5f;

        zp *= z;
        log_1_plus_z += zp * 0.333333333f;

        zp *= z;
        log_1_plus_z -= zp * 0.25f;

        zp *= z;
        log_1_plus_z += zp * 0.2f;

        zp *= z;
        log_1_plus_z -= zp * 0.16666666666f;

        zp *= z;
        log_1_plus_z += zp * 0.14285714285f;

        zp *= z;
        log_1_plus_z -= zp * 0.125f;

        zp *= z;
        log_1_plus_z += zp * 0.11111111111f;

        auto result = log_1_plus_z + convert_to<float>(n) * 0.69314718056f;
        return blend(x == 1.0f, 0.0f, result);
    }
}
