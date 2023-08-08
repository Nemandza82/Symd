#pragma once
#include "../internal/symd_register.h"


namespace symd::kernel
{
    // log(x) = log(a*2^n) = log(a) + n*log(2) = log(1 + z) + n*log(2)
    // log(2) = 0.30102999566
    // a in range [1, 2]
    // z in range [0, 1]
    // log(1 + z) = z - z^2/2 + z^3/3 ...
    template <typename T> 
    T log(T x)
    {
        // Multiply up subnormals to go into narmal range
        auto y = blend(x < 1e-37, x*100000000.0f, x);
        
        auto two_to_n = fp_2_pow_exp(y) * 2;
        auto n = convert_to<float>(fp_exp(y)) * 0.69314718056f + 0.69314718056f;
        auto a = y / two_to_n;
        auto z = a - 1;

        // For subnormals add bias since we multiplied them up
        n = blend(x < 1e-37, n-18.420680744f, n);

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

        zp *= z;
        log_1_plus_z -= zp * 0.1f;

        auto result = log_1_plus_z + n;
        return blend(x == 1.0f, 0.0f, result);
    }
}
