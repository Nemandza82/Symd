#pragma once
#include "../internal/symd_register.h"


namespace symd::kernel
{
    
    /// Computes the natural (base e) logarithm of x.
    /// Fast implementation is done by moving input to 1-2 range and then approximating logarithm with polynomial.
    template <typename T> 
    T log(T x)
    {
        /// Implemetation algorithm
        // Move input to 1-2 range and approximate with polynomial using remez algorithm
        // log(x) = log(a*2^n) = log(a) + n*log(2)
        // log(2) = 0.69314718056f
        // a in range [1, 2]
        // log(a) = -1.49278+(2.11263+(-0.729104+0.10969*a)*a)*a // 3rd order poly
        // log(a) = −1.7417939+(2.8212026+(−1.4699568+(0.44717955−0.056570851 * a)a)a)a // 4th order poly

        // Multiply up subnormals to go into narmal range
        auto y = blend(x < 1e-37, x*100000000.0f, x);
        
        auto two_to_n = exp_part_of_float(y);
        auto n = convert_to<float>(fp_exp(y)) * 0.69314718056f;
        auto a = y / two_to_n;

        // For subnormals add bias since we multiplied them up
        n = blend(x < 1e-37, n-18.420680744f, n);

        // Third order poly
        // auto log_a = a * (a * (a * 0.10969f - 0.729104f) + 2.11263f) - 1.49278f;

        // 4th order poly
        auto log_a = a * (a * (a * (a * (-0.056570851f) + 0.44717955f) - 1.4699568f) + 2.8212026f) - 1.7417939f;

        auto result = log_a + n;
        return blend(x == 1.0f, 0.0f, result);
    }
}
