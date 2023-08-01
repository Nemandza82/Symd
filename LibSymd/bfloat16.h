#pragma once
#include <array>

namespace symd
{
    // #pragma pack (1)
    struct bfloat16
    {
        short data;

        bfloat16()
        {
        }

        bfloat16(float x)
        {
            data = ((short*)(&x))[1];
        }

        float get_float() const
        {
            float res = 0.0f;
            ((short*)(&res))[1] = data;
            return res;
        }

        bfloat16 operator+(const bfloat16& other) const
        {
            return bfloat16(this->get_float() + other.get_float());
        }
    };
}
